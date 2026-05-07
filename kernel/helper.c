#include "helper.h"
#include <stdint.h>
#include "pit.h"
#include "idt.h"
#include "pic.h"

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

void uint16_to_string(uint16_t value, char *str)
{
    char temp[6];
    int i = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    // reverse into output string
    int j = 0;
    while (i > 0) {
        str[j++] = temp[--i];
    }

    str[j] = '\0';
}


void init()
{
    idt_init();
    map_pic();
    init_pit();
    irq_enable(0);
}