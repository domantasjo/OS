#pragma once
#include "console.h"
#include "editor.h"

extern bool is_editor_open;
void printchar(char c);
void delete_char(void);
void press_enter(void);
void cursor_left(void);
void cursor_down(void);
void cursor_right(void);
void cursor_up(void);
void render(void);
void toggle_editor_open(void);
void print(char *str);
