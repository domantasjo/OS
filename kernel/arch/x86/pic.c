#include "pic.h"

void irq_disable(uint8_t irq){
    uint16_t port;
    if(irq < 8)
    {
        port = PIC_1_DATA;
    }
    else{
        port = PIC_2_DATA;
        irq -= 8;
    }
    uint8_t value = inb(port);
    value |= (1 << irq);
    outb(port,value);
}

void irq_enable(uint8_t irq){
    uint16_t port;
    if(irq < 8)
    {
        port = PIC_1_DATA;
    }
    else{
        port = PIC_2_DATA;
        irq -= 8;
    }
    uint8_t value = inb(port);
    value &= ~(1 << irq);
    outb(port,value);
}

uint8_t inb(uint16_t port){
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(uint16_t port, uint8_t value){
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}