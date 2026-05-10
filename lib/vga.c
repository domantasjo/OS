#include "vga.h"
#include "../kernel/arch/x86/io.h"

volatile char *vga = (volatile char *)VGA_BUFFER;
int row = 0;
int col = 0;
volatile char console_buf[CSL_ROWS][VGA_COLS];
int viewport_top = 0;
int line_length[CSL_ROWS];

volatile char *vga_at(int row, int col) {
  return (volatile char *)VGA_BUFFER + (row * VGA_COLS * 2) + col * 2;
}

void set_cursor(int row, int col) {
  uint16_t pos = row * 80 + col;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));

  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void printchar(char character) {
  console_buf[row][col] = character;
  line_length[row]++;
  col++;
  if (col == 80) {
    if (row >= 24) {
      viewport_top++;
    }
    row++;
    col = 0;
  }
  set_cursor(row + viewport_top, col);
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
  if (row >= 24) {
    viewport_top += 1;
  }
  row++;
  set_cursor(row + viewport_top, col);
}

void delete_char(void) {
  if (col == 0) {
    console_buf[row][col] = ' ';
    col = line_length[--row];
    if (row >= 24) {
      viewport_top--;
    }
  } else {
    col--;
    console_buf[row][col] = ' ';
    line_length[row]--;
  }
  set_cursor(row + viewport_top, col);
}

int get_cols(void) { return col; }

void set_cols(int cols) { col = cols; }
