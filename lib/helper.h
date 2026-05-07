#pragma once
#include <stdint.h>

extern volatile char *VGA;
extern int row;
extern int col;
void print(char *string);
void print_nl(void);
void print_hex(uint8_t value);
void uint16_to_string(uint16_t value, char *str);
void init(void);
void printchar(char character);
extern volatile char *vga_at(int row, int col);