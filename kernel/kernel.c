#include "../drivers/keyboard.h"
#include "../lib/helper.h"
#include "../lib/terminal.h"
#include "paging.h"
#include "process.h"


void task_a()
{
  while(1)
  {
  }
}
void task_b()
{
  while(1)
  {
  }
}
void task_render()
{
  while(1)
  {
    render();
  }
}

void task_keyboard()
{
   while (true) {
    uint8_t key = keyboard_poll();
    if (key >= 0x20 && key <= 0x7E) {
      printchar(key);
    }
   }

}

void kernel_main(void) {

  init();
  print_prompt();
  
  process_init();
  process_create(task_a);
  process_create(task_b);
  process_create(task_render);
  process_create(task_keyboard);
  print("Hello");
  processes[0].state = RUNNING;
  current_process = 0;
  task_a();
}
