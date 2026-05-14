#include "vga.h"
#include "../kernel/arch/x86/io.h"
#include <stdbool.h>

volatile char *vga = (volatile char *)VGA_BUFFER;
volatile char *vga_at(int row, int col) {
  return (volatile char *)VGA_BUFFER + (row * MAX_COLS * 2) + col * 2;
}
int min(int a, int b) { return (a < b) ? a : b; }
void set_cursor(int row, int col) {
  uint16_t pos = row * 80 + col;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));

  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
void render_vga(Line lines[], int viewport_top, Cursor cursor) {
  for (int i = 0; i < VGA_ROWS; i++) {
    for (int j = 0; j < MAX_COLS; j++) {
      volatile char *cell = vga_at(i, j);

      char c = lines[i + viewport_top].chars[j];
      cell[0] = c ? c : ' ';
      cell[1] = 0x0A;
    }
  }

  set_cursor(cursor.row - viewport_top, cursor.col);
}

void clear_line(Line *line) {
  for (int i = 0; i < MAX_COLS; i++) {
    line->chars[i] = ' ';
  }
  line->line_length = 0;
}

void vga_delete_char(Cursor *cursor, Line lines[], int col_start) {
  if (cursor->row == 0 && cursor->col == 0)
    return;

  if (cursor->col > col_start) {
    cursor->col--;

    for (int i = cursor->col;
         i < lines[cursor->row].line_length - 1 + col_start; i++) {
      lines[cursor->row].chars[i] = lines[cursor->row].chars[i + 1];
    }
    lines[cursor->row].chars[lines[cursor->row].line_length - 1 + col_start] =
        ' ';
    lines[cursor->row].line_length--;
    return;
  }
}

void vga_printchar(char c, Cursor *cursor, Line lines[MAX_ROWS],
                   int col_start) {
  int r = cursor->row;

  int col = cursor->col;

  // shift right if inserting inside line
  for (int i = lines[r].line_length + col_start; i > col; i--) {
    lines[r].chars[i] = lines[r].chars[i - 1];
  }

  lines[r].chars[col] = c;
  lines[r].line_length++;

  cursor->col++;

  if (cursor->col >= MAX_COLS) {
    cursor->row++;
    cursor->col = 0;
  }
}

void print_hex(uint8_t value) {
  char hex[] = "0123456789ABCDEF";
  char str[5];
  str[0] = '0';
  str[1] = 'x';
  str[2] = hex[(value >> 4) & 0xF];
  str[3] = hex[value & 0xF];
  str[4] = 0;
}

// =====================
// New line
// =====================
