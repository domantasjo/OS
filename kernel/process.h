#pragma once
#include <stdint.h>

#define MAX_PROCESSES 16
extern int current_process;

typedef enum {
  RUNNING,
  READY,
  BLOCKED,
  ZOMBIE
} ProcessState;

typedef struct {
  uint32_t eax, ebx, ecx, edx;
  uint32_t esi, edi;
  uint32_t esp, ebp, eip;
  uint32_t eflags;
  uint32_t pid;
  ProcessState state;
  uint32_t* page_directory;
  void* kernel_stack;
} PCB;

extern void context_switch(PCB* pcb_old, PCB* pcb_new); 
extern PCB processes[MAX_PROCESSES];

void process_init();
PCB* process_create(void(*entry)());
void schedule();
void save_current_esp(uint32_t esp);
uint32_t get_next_esp();
