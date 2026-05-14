#include "../drivers/keyboard.h"
#include "../lib/helper.h"
#include "../lib/terminal.h"
#include "memory.h"
#include "paging.h"

void kernel_main(void) {

  init();
  print_prompt();
  pmm_init();
  paging_init();
  // deliberately trigger a page fault
  uint32_t* bad_ptr = (uint32_t*)0x80000000;
  *bad_ptr = 42; // this should page fault
     while (true) {
    uint8_t key = keyboard_poll();
    if (key >= 0x20 && key <= 0x7E) {
      printchar(key);
    }
    render();
  }
}
