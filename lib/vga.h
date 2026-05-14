#pragma once
#include "stdint.h"
#define VGA_BUFFER 0xB8000
#define MAX_COLS 80
#define VGA_ROWS 24
#define MAX_ROWS 1000

extern volatile char *vga;
typedef struct {
  int row;
  int col;
} Cursor;

typedef struct {
  char chars[MAX_COLS];
  int line_length;
} Line;

volatile char *vga_at(int row, int col);
int min(int a, int b);
void vga_printchar(char c, Cursor *cursor, Line lines[MAX_COLS], int col_start);
void set_cursor(int row, int col);
void render_vga(Line lines[], int viewport_top, Cursor cursor);
void vga_delete_char(Cursor *cursor, Line lines[], int col_start);
void clear_line(Line *line);
