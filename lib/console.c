#include "console.h"
#include "stdbool.h"
#include "vga.h"

static Line lines[CSL_ROWS] = {0};
static Line commands[CSL_ROWS];
static Line current_line;
static Cursor cursor = {CSL_ROW_START, CSL_COL_START};

static int viewport_top = 0;
static int cmd_head = 0; // Writing
static int cmd_tail = 0; // Reading

static void version(void);
static void clear(void);
static void print_output(char *str);

Command command_list[] = {{"-v", version}, {"clear", clear}};

//------------------------------------------------------------------

void delete_char_console(void) {
  vga_delete_char(&cursor, lines, CSL_COL_START);
}

void version(void) { print_output("Current version of this OS is v0.0.1"); }

bool check_functions(char *function) {
  for (int i = 0; function[i] != '\0'; i++) {
    if (function[i] != current_line.chars[CSL_COL_START + i])
      return false;
  }
  return true;
}

void functions() {
  for (int i = 0; i < sizeof(command_list) / sizeof(command_list[0]); i++) {
    if (check_functions(command_list[i].cmd)) {
      command_list[i].fn();
      return;
    }
  }
  print_output("Unknown command, available commands are: -v, clear");
}

void clear(void) {
  for (int i = 0; i < CSL_ROWS + viewport_top; i++) {
    clear_line(&lines[i]);
  }
}

//------------------------------------------------------------------

static void scroll(void) {
  if (cursor.row >= viewport_top + VGA_ROWS)
    viewport_top++;
}

void print_prompt(void) {
  for (int i = 0; i < CSL_COL_START; i++) {
    lines[cursor.row].chars[i] = PROMPT[i];
  }
}

static void print_output(char *str) {
  cursor.col = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    vga_printchar(str[i], &cursor, lines, 0);
  }
  cursor.row++;
  cursor.col = CSL_COL_START;
  scroll();
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
  functions();
  print_prompt();
  scroll();
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

//------------------------------------------------------------------

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
  vga_printchar(c, &cursor, lines, CSL_COL_START);
}
void render_console(void) { render_vga(lines, viewport_top, cursor); }
