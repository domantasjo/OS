#pragma once
#include <stdint.h>

typedef struct SMAP_entry {
  uint32_t addr_low;
  uint32_t addr_high;
  uint32_t length_low;
  uint32_t length_high;
  uint32_t region_type;
  uint32_t ea_bitfield; // extended attributes bitfield
} __attribute__((packed)) SMAP_entry_t;
