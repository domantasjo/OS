#pragma once
#include "stdint.h"
#define VGA_BUFFER 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25

void printchar(char character);
void print(const char *string);
uint8_t get_cols(void);

void decrement_cols(void);
volatile char *get_cell(void);
