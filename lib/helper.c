#include "helper.h"
#include "../kernel/arch/x86/idt.h"
#include "../kernel/arch/x86/io.h"
#include "../kernel/arch/x86/pic.h"
#include "../kernel/arch/x86/pit.h"
#include <stdint.h>
#include "../kernel/memory.h"

void init(void) {
  idt_init();
  map_pic();
  while (inb(0x64) & 1) { // bit 0 of status = output buffer full
    inb(0x60);            // discard byte
  }
  init_pit();
  irq_enable(0);
  irq_enable(1);
  pmm_init();
  paging_init();
  heap_init();

}
