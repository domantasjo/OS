#include "vga.h"
volatile char *vga = (volatile char *)VGA_BUFFER;
int row = 10;
int col = 0;
int line_len[VGA_ROWS];

volatile char *vga_at(int row, int col) {
  return (volatile char *)VGA_BUFFER + (row * VGA_COLS * 2) + col * 2;
}

void printchar(char character) {
  volatile char *cell = vga_at(row, col);
  cell[0] = character;
  cell[1] = 0x0A;
  col++;
  line_len[row]++;
  if (col == 80) {
    row++;
    col = 0;
  }
}

void print(const char *string) {
  for (int i = 0; string[i] != 0; i++) {
    printchar(string[i]);
  }
}

void print_nl(void) {
  row++;
  col = 0;
}

uint8_t get_cols(void) { return col; }

void decrement_cols(void) { col--; }

uint8_t get_rows(void) { return row; }

void decrement_rows(void) { row--; }

uint8_t get_line_length(void) { return line_len[row]; }

volatile char *get_cell(void) { return vga_at(row, col); }
