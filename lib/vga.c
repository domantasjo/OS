#include "vga.h"
#include "../kernel/arch/x86/io.h"
#include <stdbool.h>

#define MAX_ROWS CSL_ROWS
#define MAX_COLS VGA_COLS

// =====================
// VGA memory
// =====================
volatile char *vga = (volatile char *)VGA_BUFFER;

// =====================
// Buffer state
// =====================
static char console_buf[MAX_ROWS][MAX_COLS];
static int line_length[MAX_ROWS];
static int last_used_row;
// =====================
// Viewport
// =====================
static int viewport_top = 0;

// =====================
// Cursor model (IMPORTANT CLEANUP)
// =====================
typedef struct {
  int row;
  int col;
} Cursor;

static Cursor cursor = {0, 0};

// =====================
// VGA helpers
// =====================
static inline volatile char *vga_at(int row, int col) {
  return (volatile char *)VGA_BUFFER + (row * MAX_COLS * 2) + col * 2;
}

static inline int min(int a, int b) { return (a < b) ? a : b; }

// =====================
// Hardware cursor
// =====================
void set_cursor(int row, int col) {
  uint16_t pos = row * 80 + col;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));

  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// =====================
// Cursor movement logic
// =====================
static void move_left(void) {
  if (cursor.col > 0) {
    cursor.col--;
    return;
  }

  if (cursor.row > 0 && line_length[cursor.row - 1] > 0) {
    cursor.row--;
    cursor.col = line_length[cursor.row];
  }
}

static void move_right(void) {
  if (cursor.col < line_length[cursor.row]) {
    cursor.col++;
    return;
  }

  if (cursor.row + 1 < MAX_ROWS && line_length[cursor.row + 1] > 0) {
    cursor.row++;
    cursor.col = 0;
  }
}

// please god forgive me for the slop that i am writing
// to anyone reading this code for some reason, just now that this is a
// placeholder that is meant to just work, until i change it to a smarter
// system.

static void move_up(void) {
  if (cursor.row == 0)
    return;

  cursor.row--;
  cursor.col = min(cursor.col, line_length[cursor.row]);
}

static void move_down(void) {
  if (cursor.row + 1 >= MAX_ROWS)
    return;

  if (cursor.row + 1 <= last_used_row) {
    cursor.row++;
    cursor.col = min(cursor.col, line_length[cursor.row]);
  }
}

// =====================
// Editing
// =====================
void delete_char(void) {
  if (cursor.row == 0 && cursor.col == 0)
    return;

  if (cursor.col > 0) {
    cursor.col--;

    for (int i = cursor.col; i < line_length[cursor.row] - 1; i++) {
      console_buf[cursor.row][i] = console_buf[cursor.row][i + 1];
    }
    console_buf[cursor.row][line_length[cursor.row] - 1] = ' ';
    line_length[cursor.row]--;
    return;
  }

  // merge with previous line
  if (cursor.row > 0) {
    int prev_len = line_length[cursor.row - 1];
    int curr_len = line_length[cursor.row];

    cursor.row--;
    cursor.col = prev_len;

    for (int i = 0; i < curr_len; i++) {
      console_buf[cursor.row][prev_len + i] = console_buf[cursor.row + 1][i];
      console_buf[cursor.row + 1][i] = ' ';
    }

    line_length[cursor.row] = prev_len + curr_len;

    for (int i = cursor.row + 1; i <= last_used_row; i++) {
      for (int j = 0; j < VGA_COLS; j++) {
        console_buf[i][j] = console_buf[i + 1][j];
      }
      line_length[i] = line_length[i + 1];
    }
    last_used_row--;
  }
}

// =====================
// Character insertion
// =====================
void printchar(char c) {
  int r = cursor.row;
  int col = cursor.col;

  // shift right if inserting inside line
  for (int i = line_length[r]; i > col; i--) {
    console_buf[r][i] = console_buf[r][i - 1];
  }

  console_buf[r][col] = c;
  line_length[r]++;

  cursor.col++;

  if (cursor.col >= MAX_COLS) {
    cursor.row++;
    cursor.col = 0;
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

  console_buf[20][0] = str[0];
  console_buf[20][1] = str[1];
  console_buf[20][2] = str[2];
  console_buf[20][3] = str[3];
  console_buf[20][4] = str[4];
}

// =====================
// New line
// =====================
void print_nl(void) {
  if (cursor.row + 1 >= MAX_ROWS)
    return;

  cursor.col = line_length[++cursor.row];
  last_used_row = cursor.row;

  if (cursor.row >= 24)
    viewport_top++;
}

// =====================
// Rendering
// =====================
void render_vga(void) {
  for (int i = 0; i < VGA_ROWS; i++) {
    for (int j = 0; j < VGA_COLS; j++) {
      volatile char *cell = vga_at(i, j);

      char c = console_buf[i + viewport_top][j];
      cell[0] = c ? c : ' ';
      cell[1] = 0x0A;
    }
  }

  set_cursor(cursor.row + viewport_top, cursor.col);
}

// =====================
// Public helpers
// =====================
void print(const char *string) {
  for (int i = 0; string[i] != 0; i++) {
    printchar(string[i]);
  }
}

int get_line_length(int row) { return line_length[row]; }

int get_row(void) { return cursor.row; }

int get_col(void) { return cursor.col; }

// =====================
// Cursor movement API
// =====================
void cursor_left(void) { move_left(); }
void cursor_right(void) { move_right(); }
void cursor_up(void) { move_up(); }
void cursor_down(void) { move_down(); }
