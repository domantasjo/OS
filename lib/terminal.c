#include "terminal.h"
#include "console.h"
#include "editor.h"

void render(void) {
  if (is_editor_open) {
    render_editor();
    return;
  }
  render_console();
}

void toggle_editor_open(void) { is_editor_open = !is_editor_open; }

void print(char *str) { print_int(str); }

void printchar(char c) {
  if (is_editor_open) {
    printchar_editor(c);
    return;
  }
  printchar_console(c);
}

void delete_char(void) {
  if (is_editor_open) {
    delete_char_editor();
    return;
  }
  delete_char_console();
}

void press_enter(void) {
  if (is_editor_open) {
    press_enter_editor();
    return;
  }
  press_enter_console();
}

void cursor_left(void) {
  if (is_editor_open) {
    editor_cursor_left();
    return;
  }
  console_cursor_left();
}
void cursor_down(void) {
  if (is_editor_open) {
    editor_cursor_down();
    return;
  }
  console_cursor_down();
}

void cursor_right(void) {
  if (is_editor_open) {
    editor_cursor_right();
    return;
  }
  console_cursor_right();
}

void cursor_up(void) {
  if (is_editor_open) {
    editor_cursor_up();
    return;
  }
  console_cursor_up();
}
