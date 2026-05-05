#include "idt.h"
#include "helper.h"
#include "pic.h"

void kernel_main() {
    idt_init();
    MapPic();
    row = 10;
    print("Hello");
    print_nl();
    print("World");
    while (1) { 
    }
}



