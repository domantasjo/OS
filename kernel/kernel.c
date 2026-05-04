#include "idt.h"

static volatile char *VGA = (volatile char*) 0xB8000;
static int row,col = 0;

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

void print_nl(void){
    row++;
    col = 0;
}

void kernel_main() {
    idt_init();
    row = 10;
    print("Hello");
    print_nl();
    print("World");
    while (1) { 
    }
}



