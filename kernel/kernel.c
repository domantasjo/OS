#include "idt.h"
#include "helper.h"
#include "pic.h"
#include "pit.h"

void kernel_main() {
    row = 10;
    init();
    uint16_t seconds_old = 0;
    char buf[6];
    while(true)
    {
        uint32_t seconds = ticks / 1000;
        if(seconds_old < seconds)
        {
            uint16_to_string(seconds,buf);
            print(buf);
            print_nl();
            seconds_old = seconds;
        }
    }
}



