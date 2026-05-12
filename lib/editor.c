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

  if (cursor.row > 0 && line_length[cursor.row - 1] > 0) {
    cursor.row--;
    cursor.col = line_length[cursor.row];
  }
}

static void move_right(void) {
  if (cursor.col < line_length[cursor.row]) {
    cursor.col++;
    return;
  }

  if (cursor.row + 1 < MAX_ROWS && line_length[cursor.row + 1] > 0) {
    cursor.row++;
    cursor.col = 0;
  }
}

static void printchar_editor(char c) { printchar(c, &cursor, lines, 0); }

// please god forgive me for the slop that i am writing
// to anyone reading this code for some reason, just now that this is a
// placeholder that is meant to just work, until i change it to a smarter
// system.

static void move_up(void) {
  if (cursor.row == 0)
    return;

  cursor.row--;
  cursor.col = min(cursor.col, line_length[cursor.row]);
}

static void move_down(void) {
  if (cursor.row + 1 >= MAX_ROWS)
    return;

  if (cursor.row + 1 <= last_used_row) {
    cursor.row++;
    cursor.col = min(cursor.col, line_length[cursor.row]);
  }
}

// =====================
// Editing
// =====================
void delete_char_editor(void) {
  delete_char(&cursor, lines, 7);

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

void print_nl_editor(void) {
  if (cursor.row + 1 >= MAX_ROWS)
    return;

  cursor.col = lines[++cursor.row].line_length;
  last_used_row = cursor.row;

  if (cursor.row >= 24)
    viewport_top++;
}

void print(const char *string) {
  for (int i = 0; string[i] != 0; i++) {
    printchar(string[i], &cursor, lines, 0);
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
