#include "editor.h"
#include "vga.h"

#define MAX_ROWS CSL_ROWS
#define MAX_COLS VGA_COLS

static Line lines[MAX_ROWS];
static int line_length[MAX_ROWS];
static int last_used_row;
static int viewport_top = 0;
bool is_editor_open = false;
// =====================
// Cursor movement logic
// =====================

static Cursor cursor = {0, 0};

static void move_left(void) {
  if (cursor.col > 0) {
    cursor.col--;
    return;
  }

  if (cursor.row > 0 && lines[cursor.row - 1].line_length > 0) {
    cursor.row--;
    cursor.col = lines[cursor.row].line_length;
  }
}

static void move_right(void) {
  if (cursor.col < lines[cursor.row].line_length) {
    cursor.col++;
    return;
  }

  if (cursor.row + 1 < MAX_ROWS && lines[cursor.row + 1].line_length > 0) {
    cursor.row++;
    cursor.col = 0;
  }
}
static void move_up(void) {
  if (cursor.row == 0)
    return;

  cursor.row--;
  cursor.col = min(cursor.col, lines[cursor.row].line_length);
  if (cursor.row < viewport_top) {
    viewport_top--;
  }
}

static void move_down(void) {
  if (cursor.row + 1 >= MAX_ROWS)
    return;

  if (cursor.row + 1 <= last_used_row) {
    cursor.row++;
    cursor.col = min(cursor.col, lines[cursor.row].line_length);
  }
  if (cursor.row >= viewport_top + VGA_ROWS) {
    viewport_top++;
  }
}
void printchar_editor(char c) { vga_printchar(c, &cursor, lines, 0); }

void render_editor(void) { render_vga(lines, viewport_top, cursor); }

// please god forgive me for the slop that i am writing
// to anyone reading this code for some reason, just now that this is a
// placeholder that is meant to just work, until i change it to a smarter
// system.

// =====================
// Editing
// =====================
void delete_char_editor(void) {
  vga_delete_char(&cursor, lines, 7);

  // merge with previous line
  if (cursor.row > 0) {
    int prev_len = line_length[cursor.row - 1];
    int curr_len = line_length[cursor.row];

    cursor.row--;
    cursor.col = prev_len;

    for (int i = 0; i < curr_len; i++) {
      lines[cursor.row].chars[prev_len + i] = lines[cursor.row + 1].chars[i];
      lines[cursor.row + 1].chars[i] = ' ';
    }

    lines[cursor.row].line_length = prev_len + curr_len;

    for (int i = cursor.row + 1; i <= last_used_row; i++) {
      for (int j = 0; j < VGA_COLS; j++) {
        lines[i].chars[j] = lines[i + 1].chars[j];
      }
      lines[i].line_length = lines[i + 1].line_length;
    }
    last_used_row--;
  }
}

void press_enter_editor(void) {
  if (cursor.row + 1 >= MAX_ROWS) // If you are at the end of the buffer and
                                  // want to add a newline, return
    return;
  if (cursor.col ==
      0) { // If cursor is at col 0 and presses enter, it downshifts the whole
           // row, instead of writing a newline
    last_used_row++; // If last used row was 4, it is now 5 since everything is
                     // getting downshifted
    for (int i = last_used_row; i > cursor.row;
         i--) { // Shift every row from the bottom up to current row down by one
      lines[i] = lines[i - 1];
    }
    for (int j = 0; j < VGA_COLS; j++) // Clear the now-empty row
      lines[cursor.row].chars[j] = ' ';
    lines[cursor.row].line_length = 0;
    cursor.row++; // Move cursor onto the pushed-down original row
  } else {
    last_used_row++;
    for (int i = last_used_row; i > cursor.row + 1;
         i--) { // Shift every row below the new line down by one
      lines[i] = lines[i - 1];
    }
    int new_len =
        lines[cursor.row].line_length -
        cursor.col; // Length of text after cursor that moves to the new line
    for (int j = 0; j < new_len;
         j++) { // Move text after cursor to the new line and clear it from the
                // current line
      lines[cursor.row + 1].chars[j] = lines[cursor.row].chars[cursor.col + j];
      lines[cursor.row].chars[cursor.col + j] = ' ';
    }
    lines[cursor.row + 1].line_length = new_len;
    lines[cursor.row].line_length =
        cursor.col; // Current line now ends at the cursor position
    cursor.row++;
    cursor.col = 0;
  }
  if (cursor.row >= VGA_ROWS) {
    viewport_top++;
  } // Scroll viewport down if cursor moves past the visible area
}

void print(const char *string) {
  for (int i = 0; string[i] != 0; i++) {
    vga_printchar(string[i], &cursor, lines, 0);
  }
}

int get_line_length(int row) { return line_length[row]; }

int get_row(void) { return cursor.row; }

int get_col(void) { return cursor.col; }

// =====================
// Cursor movement API
// =====================
void editor_cursor_left(void) { move_left(); }
void editor_cursor_right(void) { move_right(); }
void editor_cursor_up(void) { move_up(); }
void editor_cursor_down(void) { move_down(); }
