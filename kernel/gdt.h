#pragma once

// GDT segment selectors. These must match the GDT layout defined in
// bootloader/bootloader.asm. A selector is the byte offset of a descriptor
// inside the GDT, with the bottom 2 bits encoding the requested privilege
// level (0 = ring 0).
//
// Current layout:
//   0x00  null descriptor
//   0x08  kernel code  (ring 0, executable)
//   0x10  kernel data  (ring 0, read/write)

#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x10
