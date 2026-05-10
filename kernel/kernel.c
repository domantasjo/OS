#include "../drivers/keyboard.h"
#include "../lib/helper.h"
#include "../lib/vga.h"

void kernel_main(void) {
  init();
  while (true) {
    uint8_t key = keyboard_poll();
    if (key >= 0x20 && key <= 0x7E) {
      printchar(key);
    }
    render_vga();
  }
}
