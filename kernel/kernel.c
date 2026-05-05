#include "idt.h"
#include "helper.h"
#include "pic.h"

void kernel_main() {
    row = 10;
    idt_init();
    map_pic();
    uint8_t before = inb(PIC_1_DATA);
    print_hex(before);
    print_nl();

    irq_enable(1);
    uint8_t after1 = inb(PIC_1_DATA);
    print_hex(after1);
    print_nl();

    irq_disable(1);
    uint8_t after2 = inb(PIC_1_DATA);
    print_hex(after2);
    print_nl();
    while (1) { 
    }
}



