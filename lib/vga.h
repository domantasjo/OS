#pragma once
#include "stdint.h"
#define VGA_BUFFER 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25
#define CSL_ROWS 1000

void printchar(char character);
void print(const char *string);
void print_nl(void);
int get_cols(void);
void set_cols(int cols);
void render_vga(void);
void print_nl(void);
void delete_char(void);
volatile char *get_cell(void);
