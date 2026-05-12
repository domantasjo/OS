#pragma once

#define CSL_COL_START 7
#define CSL_ROW_START 1
#define PROMPT "pc ~ $ "

void delete_char_console(void);
void press_enter_console(void);

void console_cursor_left(void);
void console_cursor_right(void);
void console_cursor_up(void);
void console_cursor_down(void);
void printchar_console(char c);
void render_console(void);
void print_prompt(void);
