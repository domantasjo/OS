#pragma once

extern volatile char *VGA;
extern int row;
extern int col;
void print(char *string);
void print_nl(void);