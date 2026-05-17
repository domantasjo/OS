#include "process.h"
#include "memory.h"
#include "paging.h"

PCB processes[MAX_PROCESSES];
int current_process = 0;
int process_count = 0;

void process_init()
{
  for(int i = 0; i < MAX_PROCESSES; i++)
  {
    processes[i].state = ZOMBIE;
  }
}

PCB* process_create(void (*entry)())
{
  int slot = -1;

  for(int i = 0; i < MAX_PROCESSES;i++)
  {
    if(processes[i].state == ZOMBIE)
    {
      slot = i;
      break;
    }
  }
  if(slot == -1) return NULL;
  void* stack= kmalloc(4096);
  uint32_t* stack_top = (uint32_t*)((uintptr_t)stack+4096);
  
// CPU pushes these during interrupt
  *--stack_top = 0x202;          // >eflags
  *--stack_top = 0x08;           // cs (your code segment)
  *--stack_top = (uint32_t)entry; // eip

  // your macro pushes these
  *--stack_top = 0;              // err
  *--stack_top = 0;              // vector

  // pusha pushes these (8 registers, all 0)
  *--stack_top = 0;              // edi
  *--stack_top = 0;              // esi
  *--stack_top = 0;              // ebp
  *--stack_top = 0;              // esp_old
  *--stack_top = 0;              // ebx
  *--stack_top = 0;              // edx
  *--stack_top = 0;              // ecx
  *--stack_top = 0;              // eax

  // stack_top now points to the top of the fake frame
  PCB* process = &processes[slot];
  process->esp = (uint32_t)stack_top;
  process->eax = 0;
  process->ebx = 0;
  process->ecx = 0;
  process->edx = 0;
  process->esi = 0;
  process->edi = 0;
  process->esp = (uint32_t)(uintptr_t)stack_top;
  process->ebp = (uint32_t)(uintptr_t)stack + 4096; 
  process->eip = (uint32_t)(uintptr_t)entry;
  process->eflags = 0x202;
  process->kernel_stack = stack;
  process->page_directory = page_directory;
  process->pid = slot;
  process->state = READY;
  process_count++;
  return process;
}

void schedule()
{
  processes[current_process].state = READY;
  for(int i = 1; i < MAX_PROCESSES; i++)
  {
    int next = (current_process + i) % MAX_PROCESSES;
    if(processes[next].state == READY && next != current_process)
    {
      current_process = next;
      processes[next].state = RUNNING;
      return;
    }
  }
}

void save_current_esp(uint32_t esp) {
    processes[current_process].esp = esp;
}

uint32_t get_next_esp() {
    return processes[current_process].esp;
}
