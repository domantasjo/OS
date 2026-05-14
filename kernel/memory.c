#include "memory.h"
#include "../lib/string.h"
#include "../lib/terminal.h"

uint8_t* bitmap;
uint32_t total_pages;

void bitmap_clear(uint32_t page);
void pmm_mark_used(uint32_t page_start, uint32_t size);

void pmm_init(void)
{
  bitmap = (uint8_t*)((uintptr_t)&kernel_end + 0xFFF & ~0xFFF);
  SMAP_entry_t *smap = (SMAP_entry_t *)0x0504;
  const int smap_size = 0x2000;
  uint16_t entries = *(volatile uint16_t *)0x0500;
  uint64_t highest = 0;
  for(uint16_t i = 0; i < entries; i++)
  {
    if(smap[i].region_type != 1) continue;

    uint64_t base   = ((uint64_t)smap[i].addr_high << 32) | smap[i].addr_low;
    uint64_t length = ((uint64_t)smap[i].length_high << 32) | smap[i].length_low;
    uint64_t end    = base + length;  
    
    if(end > highest)
      highest = end;
    
  }


  total_pages = highest/ 4096;

  memset(bitmap,0xFF,total_pages/8);

  

  for(uint16_t i = 0; i < entries; i++)
  {
    if(smap[i].region_type != 1) continue;

    uint64_t base   = ((uint64_t)smap[i].addr_high << 32) | smap[i].addr_low;
    uint64_t length = ((uint64_t)smap[i].length_high << 32) | smap[i].length_low;
   
    uint32_t start_page = base/4096;
    uint32_t end_page = (base+length)/4096;

    for(uint32_t p = start_page; p < end_page; p++)
    {
      bitmap_clear(p);
    }
  }
    pmm_mark_used(0x0000, 0x1000);
    pmm_mark_used(0x1000, (uintptr_t)&kernel_end - 0x1000);
    pmm_mark_used((uintptr_t)bitmap, total_pages / 8);
    pmm_mark_used(0x0500, sizeof(SMAP_entry_t) * entries + 2);
    pmm_mark_used(0x80000, 0x10000);
}

void bitmap_set(uint32_t page)
{
  bitmap[page/8] |= (1 << (page % 8));
}

void bitmap_clear(uint32_t page)
{
  bitmap[page/8] &= ~(1 << (page % 8));
}

bool bitmap_test(uint32_t page) {
    return bitmap[page / 8] & (1 << (page % 8));
}

void* alloc_page(void)
{
  for(uint32_t i = 0; i < total_pages; i++)
  {
    if(!bitmap_test(i))
    {
      bitmap_set(i);
      return (void*)(uintptr_t)(i*4096);
    }
  }  return NULL;
}

void free_page(void* addr)
{
  uint32_t page = (uintptr_t)addr / 4096;
  bitmap_clear(page);
}

void* memset(void* ptr, int value, size_t size)
{
  uint8_t* p = (uint8_t*)ptr;
  for(uint32_t i = 0; i < size; i++)
  {
    p[i] = (uint8_t)value;
  }
  return ptr;
}

void pmm_mark_used(uint32_t start, uint32_t size)
{
  uint32_t start_page = start/4096;
  uint32_t end_page = (start + size + 0xFFF) / 4096;
  for(uint32_t p = start_page; p < end_page; p++)
  {
    bitmap_set(p);
  }
}
