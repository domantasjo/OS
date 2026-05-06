#include "idt.h"
#include "gdt.h"
#include "helper.h"
#include <stdint.h>
#include "pic.h"

static idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static idtr_t idtr;
extern void* isr_stub_table[];

void interrupt_handler(uint32_t vector, uint32_t err){
    if(vector < 32)
    {
        switch(vector){
            case 0:
            print("Division by zero");
            break;
            case 6: 
            print("Invalid opcode");
            break;
            case 13:
            print("General protection fault");
            break;
            case 14:
            print("Page fault");
            break;
         }
        __asm__ volatile("cli;hlt");
    }
    else if(vector < 40)
    {
        if(vector == 32)
        {
            print("tick");
            print_nl();
        }
        outb(PIC_1_CTRL,0x20);
    }
    else
    {
        outb(PIC_2_CTRL,0x20);
        outb(PIC_1_CTRL,0x20);
    }

}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags){
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = KERNEL_CODE_SEGMENT;
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t) isr >> 16;
    descriptor->reserved = 0;
}

void idt_init(void){
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for(uint8_t vector = 0; vector < 48; vector++){
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_FLAG_INTERRUPT_GATE);
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr));
}


