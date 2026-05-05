#include "helper.h"
#include <stdint.h>

volatile char *VGA = (volatile char*) 0xB8000;
int row = 0;
int col = 0;

static volatile char *vga_at(int row, int col){
    return (volatile char*) 0xB8000 + (row*80*2) + col*2;
}

void print(char *string){
    for (int i = 0; string[i] != 0; i++) {
        volatile char *cell = vga_at(row,col);
        cell[0] = string[i];
        cell[1] = 0x0A;
        col++;
        if(col == 80)
        {
            row++;
            col = 0;
        }
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

    print(str);
}

void print_nl(void){
    row++;
    col = 0;
}