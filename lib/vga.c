#include "vga.h"
volatile char *vga = (volatile char *)VGA_BUFFER;
int row = 0;
int col = 0;
volatile char console_buf[CSL_ROWS][VGA_COLS];
int viewport_top = 0;
int line_length[CSL_ROWS];

volatile char *vga_at(int row, int col) {
  return (volatile char *)VGA_BUFFER + (row * VGA_COLS * 2) + col * 2;
}

void printchar(char character) {
  console_buf[row][col] = character;
  line_length[row]++;
  col++;
  if (col == 80) {
    row++;
    col = 0;
  }
}

void render_vga(void) {
  for (int i = 0; i < VGA_ROWS; i++) {
    for (int j = 0; j < VGA_COLS; j++) {
      volatile char *cell = vga_at(i, j);
      cell[0] = console_buf[i + viewport_top][j];
      cell[1] = 0x0A;
    }
  }
}

void print(const char *string) {
  for (int i = 0; string[i] != 0; i++) {
    printchar(string[i]);
  }
}

void print_nl(void) {
  col = 0;
  row++;
}

void delete_char(void) {
  if (col == 0) {
    console_buf[row][col] = ' ';
    col = line_length[--row];
  } else {
    col--;
    console_buf[row][col] = ' ';
    line_length[row]--;
  }
}

int get_cols(void) { return col; }

void set_cols(int cols) { col = cols; }
