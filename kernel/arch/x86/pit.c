#include "io.h"
#include <stdint.h>

volatile uint32_t ticks = 0;

void init_pit(void) {
  uint16_t divisor = 1193;
  outb(0x43, 0x34);
  outb(0x40, divisor & 0xFF);
  outb(0x40, (divisor >> 8) & 0xFF);
}
