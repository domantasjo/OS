#include "../drivers/keyboard.h"
#include "../lib/helper.h"
#include "../lib/terminal.h"
#include "paging.h"

void kernel_main(void) {

  init();
  print_prompt();
  // deliberately trigger a page fault
  print("Hello World");
     while (true) {
    uint8_t key = keyboard_poll();
    if (key >= 0x20 && key <= 0x7E) {
      printchar(key);
    }
    render();
  }
}
