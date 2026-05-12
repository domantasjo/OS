#include "console.h"
#include "stdbool.h"
#include "vga.h"

Line lines[CSL_ROWS] = {0};
Line commands[CSL_ROWS];
Line current_line;
int cmd_head = 0; // Writing
int cmd_tail = 0; // Reading
Cursor cursor = {CSL_ROW_START, CSL_COL_START};
int viewport_top = 0;

void delete_char_console(void) { vga_delete_char(&cursor, lines, 7); }

void print_prompt(void) {
  Cursor old_cursor = cursor;
  cursor.col = 0;
  for (int i = 0; i < 7; i++) {
    lines[cursor.row].chars[i] = PROMPT[i];
  }
  cursor.col = old_cursor.col;
}

static void print_output(char *str) {
  cursor.col = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    vga_printchar(str[i], &cursor, lines, 0);
  }
  cursor.row++;
  cursor.col = CSL_COL_START;
}

void version(void) { print_output("Current version of this OS is v0.0.1"); }

bool check_functions(char *function) {
  for (int i = 0; function[i] != '\0'; i++) {
    if (function[i] != current_line.chars[CSL_COL_START + i])
      return false;
  }
  return true;
}

void clear(void) {
  for (int i = 0; i < CSL_ROWS + viewport_top; i++) {
    clear_line(&lines[i]);
  }
}

void press_enter_console(void) {
  if (cursor.row + 1 >= CSL_ROWS)
    return;

  cursor.col = CSL_COL_START;
  if (lines[cursor.row].line_length > 0) {
    commands[cmd_head++] = lines[cursor.row];
    cmd_tail++;
  }
  cursor.row++;
  if (check_functions("-v")) {
    version();
  } else if (check_functions("clear")) {
    clear();
  } else {
    print_output("Unknown command. Current available commands are: -v, clear");
  }
  print_prompt();

  if (cursor.row >= viewport_top + CSL_ROWS)
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
  vga_printchar(c, &cursor, lines, 7);
}
void render_console(void) { render_vga(lines, viewport_top, cursor); }
