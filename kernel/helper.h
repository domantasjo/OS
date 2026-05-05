#pragma once
#include <stdint.h>

extern volatile char *VGA;
extern int row;
extern int col;
void print(char *string);
void print_nl(void);
void print_hex(uint8_t value);