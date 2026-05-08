#pragma once
#include "stdint.h"
#define VGA_BUFFER 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

void printchar(char character);
void print(const char *string);
uint8_t get_cols(void);
void print_nl(void);

void decrement_cols(void);
uint8_t get_rows(void);
void decrement_rows(void);
uint8_t get_line_length(void);
volatile char *get_cell(void);
