#include "console.h"
#include "vga.h"

Line lines[CSL_ROWS] = {0};
Line commands[CSL_ROWS];
Line current_line;
int cmd_head = 0; // Writing
int cmd_tail = 0; // Reading
Cursor cursor = {CSL_ROW_START, CSL_COL_START};
int viewport_top = 0;

void delete_char_console(void) { delete_char(&cursor, lines, 7); }

void print_prompt() {
  Cursor old_cursor = cursor;

  cursor.col = 0;
  for (int i = 0; i < 7; i++) {
    lines[cursor.row].chars[i] = PROMPT[i];
  }
  cursor.col = old_cursor.col;
}

void print_nl_console(void) {
  if (cursor.row + 1 >= CSL_ROWS)
    return;

  cursor.col = CSL_COL_START;
  if (lines[cursor.row].line_length > 0) {
    commands[cmd_head++] = lines[cursor.row];
    cmd_tail++;
  }
  cursor.row++;
  print_prompt();

  if (cursor.row >= 24)
    viewport_top++;

  clear_line(&current_line);
}

static void move_left(void) {
  if (cursor.col > CSL_COL_START) {
    cursor.col--;
    return;
  }
}

static void move_right(void) {
  if (cursor.col < lines[cursor.row].line_length + CSL_COL_START) {
    cursor.col++;
    return;
  }
}

void console_cursor_left(void) { return move_left(); }
void console_cursor_right(void) { return move_right(); }
void console_cursor_up(void) {
  if (cmd_tail > 0) {
    lines[cursor.row] = commands[--cmd_tail];
    cursor.col = lines[cursor.row].line_length + CSL_COL_START;
  }
}
void console_cursor_down(void) {
  if (cmd_tail < cmd_head - 1) {
    lines[cursor.row] = commands[++cmd_tail];
  } else {
    cmd_tail = cmd_head;
    lines[cursor.row] = current_line;
    print_prompt();
  }
  cursor.col = lines[cursor.row].line_length + CSL_COL_START;
}

void printchar_console(char c) {
  current_line.chars[cursor.col] = c;
  current_line.line_length++;
  printchar(c, &cursor, lines, 7);
}
void render_vga_console(void) { render_vga(lines, viewport_top, cursor); }
