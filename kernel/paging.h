#include <stdint.h>

extern uint32_t* page_directory;
extern void paging_enable(uint32_t* page_dir); 
void paging_init(void);
