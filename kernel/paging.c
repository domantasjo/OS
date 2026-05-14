#include "paging.h"
#include "memory.h"

uint32_t* page_directory;
uint32_t* page_table;

void paging_init(void)
{
  page_directory = alloc_page();
  page_table = alloc_page();
  memset(page_directory,0,4096);
  memset(page_table,0,4096);

  for(uint32_t i = 0; i < 1024; i++)
  {
    page_table[i] = (i * 4096) | 0x3;
  }
  page_directory[0] = (uint32_t)(intptr_t)page_table | 0x3;

  paging_enable(page_directory);
}
