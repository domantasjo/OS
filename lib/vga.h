#pragma once
#include "stdint.h"
#define VGA_BUFFER 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25
#define CSL_ROWS 1000

void printchar(char character);
void print(const char *string);
void print_nl(void);
void render_vga(void);
void print_nl(void);
void delete_char(void);
int get_line_length(int row);
int get_row(void);
int get_col(void);
void cursor_left(void);
void cursor_right(void);
void cursor_up(void);
void cursor_down(void);
