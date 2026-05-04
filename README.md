# os_from_scratch

A small Unix-like operating system being built from scratch, x86 (32-bit), xv6-style. See `roadmap.md` for the full plan and progress.

## Current state

- 16-bit real-mode bootloader that loads the kernel from disk and switches to 32-bit protected mode
- Minimal protected-mode kernel that prints to VGA text memory
- Interrupt Descriptor Table (IDT) catching the first 32 CPU exceptions

## Layout

```
bootloader/    Stage-1 boot sector + helpers (real mode asm)
kernel/        Protected-mode kernel (C + asm)
  entry.asm    Trampoline that calls kernel_main
  kernel.c     Kernel entry point and VGA text output
  idt.{c,asm,h}  Interrupt Descriptor Table and exception stubs
  gdt.h        Segment selector constants
make/          Build files (linker script + makefile)
build/         Build artifacts (generated)
```

## Build & run

Requires `nasm`, `i686-elf-gcc` cross-compiler, and `qemu-system-i386`.

```bash
make            # build build/os-image.bin
make run        # build and boot it under QEMU
make clean
```

## Boot chain

```
BIOS
  -> boot sector (real mode)
  -> load kernel sectors from disk (int 13h)
  -> enable A20, load GDT, switch to protected mode
  -> jump to kernel at 0x1000
  -> kernel_main: idt_init() then VGA output
```
