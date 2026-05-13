#pragma once

#define CSL_COL_START                                                          \
  7 // Which column the output starts at in a console (7, because pc ~ $ is 7
    // chars long, so it starts after that)
#define CSL_ROW_START 1 // Starts at 1 column below top
#define PROMPT                                                                 \
  "pc ~ $ " // Prompt that goes before output, like pc ~ $ [user input]

typedef struct {
  char *cmd;
  void (*fn)(void);
} Command;

void delete_char_console(void);
void press_enter_console(void);
void console_cursor_left(void);
void console_cursor_right(void);
void console_cursor_up(void);
void console_cursor_down(void);
void printchar_console(char c);
void render_console(void);
void print_prompt(void);
void print_int(char *str);
