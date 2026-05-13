#include "../drivers/keyboard.h"
#include "../lib/helper.h"
#include "../lib/terminal.h"
#include "memory.h"

void kernel_main(void) {

  init();
  print_prompt();
  SMAP_entry_t *smap = (SMAP_entry_t *)0x0504;
  const int smap_size = 0x2000;

  uint32_t entries = *(volatile uint32_t *)0x0500;
  while (true) {
    uint8_t key = keyboard_poll();
    if (key >= 0x20 && key <= 0x7E) {
      printchar(key);
    }
    render();
  }
}
