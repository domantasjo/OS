#include "pic.h"
#include "io.h"

void irq_disable(uint8_t irq) {
  uint16_t port;
  if (irq < 8) {
    port = PIC_1_DATA;
  } else {
    port = PIC_2_DATA;
    irq -= 8;
  }
  uint8_t value = inb(port);
  value |= (1 << irq);
  outb(port, value);
}

void irq_enable(uint8_t irq) {
  uint16_t port;
  if (irq < 8) {
    port = PIC_1_DATA;
  } else {
    port = PIC_2_DATA;
    irq -= 8;
  }
  uint8_t value = inb(port);
  value &= ~(1 << irq);
  outb(port, value);
}

bool is_spurious_irq(uint16_t port) {
  outb(port, 0x0B);
  uint8_t isr = inb(port);
  return !(isr & (1 << 7));
}
