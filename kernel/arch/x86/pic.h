#pragma once
#include <stdint.h>

#define PIC_1_CTRL 0x20
#define PIC_2_CTRL 0xA0
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1
#define IRQ_0 0x20
#define IRQ_8 0x28     

extern void map_pic(void);
void irq_disable(uint8_t irq);
void irq_enable(uint8_t irq);
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);