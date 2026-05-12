#pragma once
#include <stdbool.h>

extern bool is_editor_open;

void editor_cursor_up(void);
void editor_cursor_down(void);
void editor_cursor_left(void);
void editor_cursor_right(void);
void delete_char_editor(void);
void print_nl_editor(void);
static void printchar_editor(char c);
