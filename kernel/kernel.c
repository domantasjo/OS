#include "../drivers/keyboard.h"
#include "../lib/console.h"
#include "../lib/helper.h"

void kernel_main(void) {
  init();
  print_prompt();
  while (true) {
    uint8_t key = keyboard_poll();
    if (key >= 0x20 && key <= 0x7E) {
      printchar_console(key);
    }

    render_vga_console();
  }
}
