#include "idt.h"
#include "helper.h"
#include "pic.h"
#include "pit.h"
#include "keyboard.h"

void kernel_main() {
    row = 10;
    init();
    uint16_t seconds_old = 0;
    char buf[6];
    while(true)
    {
        uint8_t key = keyboard_poll();
        if (key >= 0x20 && key <= 0x7E)
        {
            printchar(key);
        }
    }
}



