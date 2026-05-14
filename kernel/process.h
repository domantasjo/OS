#pragma once
#include <stdint.h>

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
