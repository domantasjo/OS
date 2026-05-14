#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern uint32_t kernel_end;

typedef struct SMAP_entry {
  uint32_t addr_low;
  uint32_t addr_high;
  uint32_t length_low;
  uint32_t length_high;
  uint32_t region_type;
  uint32_t ea_bitfield; // extended attributes bitfield
} __attribute__((packed)) SMAP_entry_t;

void pmm_init(void);
void* alloc_page(void);
void free_page(void* addr);
bool bitmap_test(uint32_t page);
void* memset(void* ptr, int value, size_t size);
void heap_init(void);
void* kmalloc(uint32_t size);
void paging_init(void);
