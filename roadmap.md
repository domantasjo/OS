# Building a Small Unix-Like Operating System (xv6-style)

A step-by-step roadmap for building a small Unix-like OS similar in scope to xv6,
starting from a bare boot sector.

---

## Big Picture Architecture

```
Bootloader -> Protected Mode -> Kernel -> Drivers -> Memory -> Processes -> Filesystem -> Shell
```

You are finished when you can:

* run multiple processes
* read and write files
* execute programs
* interact through a shell

---

# PHASE 1 -- Stage 1 Bootloader

The boot sector is the first code the BIOS runs. Before loading a kernel you need
a working bootloader that can at least print to the screen and read disk sectors.

## [X] Step 1: Print Text in Real Mode

Make the boot sector print a message using BIOS video services.

Implement:

* set video mode / cursor position
* print a string using `int 0x10` (teletype output, AH=0x0E)

Learn:

* real-mode segmentation (CS, DS, ES, SS)
* BIOS interrupt calling convention

Goal: Boot sector prints "Hello" on screen and halts.

---

## [X] Step 2: Read Disk Sectors

The boot sector is only 512 bytes -- everything else must be loaded from disk.

Implement:

* read sectors using BIOS `int 0x13` (AH=0x02)
* understand CHS addressing (or use LBA with extension int 13h AH=0x42)
* load sectors to a known address (e.g. `0x7E00` right after the boot sector, or `0x1000`)
* verify the read succeeded (carry flag / AH error code)

Goal: Bootloader reads additional sectors from disk into memory.

---

# PHASE 2 -- Build Environment

Before writing a kernel in C you need a proper cross-compilation toolchain.
Set this up now so the kernel work in Phase 3 goes smoothly.

## [X] Step 3: Cross-Compiler & Build System

Set up:

* cross-compiler targeting i686-elf (`i686-elf-gcc`, `i686-elf-ld`)
* a linker script that places the kernel at a known address (e.g. `0x1000`)
* a Makefile that assembles the bootloader, compiles the kernel, and produces
  a bootable disk image

Learn:

* freestanding C (no stdlib, `-ffreestanding -nostdlib`)
* linker scripts (ENTRY, SECTIONS, memory layout)

Goal: `make` produces a disk image you can boot in QEMU.

---

# PHASE 3 -- Protected Mode & Kernel Entry

## [X] Step 4: Write a Minimal Kernel

Create a tiny kernel in C (or assembly) that proves it is running:

* write directly to the VGA text buffer at `0xB8000`
* print a message
* halt the CPU (`hlt` loop)

At first, test by embedding it with the bootloader or by linking it in flat
binary. You will integrate it with the full boot chain in the next steps.

Goal: Kernel code writes to the screen.

---

## [X] Step 5: Switch to Protected Mode

Real mode limits you to 1 MB of memory and 16-bit code.

Steps (done in the bootloader, after loading the kernel sectors):

1. Disable interrupts (`cli`)
2. Enable the A20 line (keyboard controller or fast A20)
3. Load a Global Descriptor Table (GDT) with code and data segments
4. Set the PE bit in CR0
5. Far jump to a 32-bit code segment

Why:

* access the full 4 GB address space
* run 32-bit C code
* prerequisite for paging and privilege rings

Goal: CPU is in 32-bit protected mode.

---

## [X] Step 6: Jump to Kernel

After switching to protected mode, jump to the kernel entry point at the address
where the bootloader loaded it (e.g. `0x1000`).

At this point the full boot chain is:

```
BIOS -> boot sector (real mode)
     -> load kernel sectors from disk
     -> switch to protected mode
     -> jump to kernel
     -> kernel prints to VGA and halts
```

Goal: Complete boot-to-kernel handoff.

---

# PHASE 4 -- Interrupts & Hardware Communication

## [X] Step 7: Setup IDT (Interrupt Descriptor Table)

Create and load an IDT.

Handle CPU exceptions:

* divide by zero (#DE)
* invalid opcode (#UD)
* general protection fault (#GP)
* page fault (#PF) -- stub for now

Goal: Kernel catches CPU exceptions instead of triple-faulting.

---

## [X] Step 8: Remap the PIC & Enable Hardware Interrupts

The default PIC mapping overlaps CPU exceptions (IRQ 0-7 map to interrupts 0-7).

Implement:

* remap PIC so IRQs start at interrupt 32+
* enable/disable individual IRQs
* write an IRQ dispatch handler

Goal: Hardware interrupts arrive without conflicting with exceptions.

---

## [X] Step 9: Timer (PIT)

Configure the Programmable Interval Timer (PIT) to fire periodic interrupts.

Use it to:

* count ticks since boot
* drive preemptive scheduling later

Goal: Periodic timer interrupt fires and increments a tick counter.

---

# PHASE 5 -- Keyboard & Console I/O

## [X] Step 10: Keyboard Driver

Read scancodes from I/O port `0x60` in the IRQ 1 handler.

Implement:

* scancode set 1 -> ASCII translation table
* modifier key tracking (shift, ctrl)

Goal: Kernel receives and decodes keystrokes.

---

## [ ] Step 11: Console / TTY

Implement:

* a scrolling VGA text-mode console
* `putchar()`, `puts()`, `printf()`-style output
* input buffer with line editing (backspace, enter)

Goal: Interactive console -- user types, OS echoes and buffers input.

---

# PHASE 6 -- Memory Management

## [ ] Step 12: Detect Physical Memory

Use BIOS `int 0x15, E820` (called before entering protected mode) or assume
a safe usable region (e.g. 1 MB - 16 MB).

Store a memory map for the physical memory manager.

---

## [ ] Step 13: Physical Page Allocator

Manage 4 KB physical pages:

* start with a simple bitmap or free-list allocator
* `alloc_page()` / `free_page()`

Goal: Kernel can allocate and free physical pages.

---

## [ ] Step 14: Paging & Virtual Memory

Set up:

* page directory + page tables
* identity-map the kernel
* map the VGA buffer
* enable paging (set PG bit in CR0)

Goal: Kernel runs with virtual memory enabled; page faults are caught by the IDT.

---

## [ ] Step 15: Kernel Heap (kmalloc)

Build a heap allocator on top of the page allocator:

* start with a bump / watermark allocator
* later upgrade to a free-list or slab allocator

Goal: `kmalloc(size)` / `kfree(ptr)` for arbitrary-size kernel allocations.

---

# PHASE 7 -- Processes & Scheduling

## [ ] Step 16: Process Control Block (PCB)

Define a process structure:

* saved registers (EAX-EDI, ESP, EBP, EIP, EFLAGS)
* kernel stack
* page directory pointer
* process state (RUNNING, READY, BLOCKED, ZOMBIE)
* PID

Goal: Data structures to represent a process.

---

## [ ] Step 17: Context Switching

Implement saving and restoring CPU state:

* save current registers to outgoing PCB
* load registers from incoming PCB
* switch page directories if using per-process address spaces

Start with cooperative multitasking (explicit `yield()`).

Goal: Two kernel tasks switch back and forth.

---

## [ ] Step 18: Preemptive Round-Robin Scheduler

Use the timer interrupt (Step 9) to:

* preempt the running process
* pick the next READY process
* context-switch to it

Goal: Multiple processes share the CPU automatically.

---

# PHASE 8 -- User Mode & System Calls

## [ ] Step 19: Ring 3 (User Mode)

Set up:

* TSS (Task State Segment) for kernel stack on privilege switch
* user-mode code/data GDT segments (DPL=3)
* separate user stack
* `iret` to ring 3

Goal: A process runs in user mode; accessing kernel memory causes a fault.

---

## [ ] Step 20: System Calls

Allow user programs to request kernel services via `int 0x80`:

Essential syscalls:

* `write(fd, buf, len)`
* `read(fd, buf, len)`
* `exit(status)`
* `fork()`
* `exec(path, argv)`
* `wait()`
* `getpid()`

Goal: User programs interact with the kernel through a defined interface.

---

# PHASE 9 -- Disk & File System

## [ ] Step 21: ATA/IDE Disk Driver

Read and write 512-byte sectors using PIO mode ATA:

* I/O ports `0x1F0`-`0x1F7`
* identify drive, read sectors, write sectors

Goal: Kernel can read/write raw disk sectors in protected mode.

---

## [ ] Step 22: File System

Choose one approach:

### Option A -- Simple (FAT-like)

* read a FAT12/FAT16 volume

### Option B -- xv6-style

* superblock
* inode table
* directory entries
* block bitmap
* `open()`, `read()`, `write()`, `close()`, `mkdir()`, `link()`

Goal: Structured, persistent file storage.

---

# PHASE 10 -- Program Loading & Shell

## [ ] Step 23: ELF Loader

Parse ELF headers and load program segments into a new address space.

Set the entry point and jump to user mode.

Goal: Kernel can load and run an external binary.

---

## [ ] Step 24: fork() and exec()

Implement:

* `fork()` -- duplicate the current process (copy page tables / COW later)
* `exec()` -- replace current process image with a new ELF binary
* `wait()` -- parent waits for child to exit

Goal: Process creation and program execution work.

---

## [ ] Step 25: Shell

A minimal interactive shell:

* read a line from console
* parse command and arguments
* `fork()` + `exec()` the command
* built-in commands: `cd`, `exit`

Goal: User can type commands and run programs.

---

# PHASE 11 -- Userland Utilities

Write small programs that use your syscall interface:

* `echo` -- print arguments
* `cat` -- print file contents
* `ls` -- list directory entries
* `mkdir` -- create directory
* `ps` -- list running processes
* `kill` -- send signal / terminate process
* `wc` -- count lines/words

Goal: A functional, self-contained Unix-like environment.

---

# Tools & Debugging

| Tool | Purpose |
|------|---------|
| QEMU | fast emulation, `-s -S` for GDB stub |
| Bochs | cycle-accurate, built-in debugger |
| GDB | `target remote :1234`, break at kernel entry |
| objdump | disassemble kernel ELF |
| hexdump / xxd | inspect raw disk images |
| serial port | early debug output before VGA driver |
