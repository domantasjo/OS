root  := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
build := $(root)/build
cc    := i686-elf-gcc
ld    := i686-elf-ld
as    := nasm
cflags := -ffreestanding -m32 -nostdlib -nostartfiles -nodefaultlibs \
          -I$(root)/kernel -I$(root)/kernel/arch/x86 \
          -I$(root)/drivers -I$(root)/lib \
          -MMD -MP
ldflags := -T $(root)/make/linker.ld --oformat binary

objs := \
    $(build)/kernel/entry.o \
    $(build)/kernel/kernel.o \
    $(build)/kernel/arch/x86/idt.o \
    $(build)/kernel/arch/x86/idt_asm.o \
    $(build)/kernel/arch/x86/pic.o \
    $(build)/kernel/arch/x86/pic_asm.o \
    $(build)/kernel/arch/x86/pit.o \
    $(build)/drivers/keyboard.o \
    $(build)/lib/helper.o \
    $(build)/lib/fmt.o \
    $(build)/lib/vga.o \
    $(build)/lib/console.o \
    $(build)/lib/editor.o \
		$(build)/lib/terminal.o \
		$(build)/lib/string.o \
		$(build)/kernel/memory.o \
		$(build)/kernel/paging_asm.o \
		$(build)/kernel/paging.o

all: $(build)/os-image.bin

# ── Bootloader ────────────────────────────────────────────────────────────────
$(build)/boot.bin: $(root)/bootloader/bootloader.asm \
                   $(root)/bootloader/print.asm \
                   $(root)/bootloader/disk.asm
	mkdir -p $(dir $@)
	$(as) -f bin -i $(root)/bootloader/ $< -o $@

# ── C sources ─────────────────────────────────────────────────────────────────
$(build)/%.o: $(root)/%.c
	mkdir -p $(dir $@)
	$(cc) $(cflags) -c $< -o $@

# ── ASM sources ───────────────────────────────────────────────────────────────
$(build)/kernel/entry.o: $(root)/kernel/entry.asm
	mkdir -p $(dir $@)
	$(as) -f elf $< -o $@

$(build)/kernel/arch/x86/idt_asm.o: $(root)/kernel/arch/x86/idt.asm
	mkdir -p $(dir $@)
	$(as) -f elf $< -o $@

$(build)/kernel/arch/x86/pic_asm.o: $(root)/kernel/arch/x86/pic.asm
	mkdir -p $(dir $@)
	$(as) -f elf $< -o $@

$(build)/kernel/paging_asm.o: $(root)/kernel/paging.asm
	mkdir -p $(dir $@)
	$(as) -f elf $< -o $@

# ── Link ──────────────────────────────────────────────────────────────────────
$(build)/kernel.bin: $(objs) $(root)/make/linker.ld
	$(ld) $(ldflags) -o $@ $(objs)

# ── Final image ───────────────────────────────────────────────────────────────
$(build)/os-image.bin: $(build)/boot.bin $(build)/kernel.bin
	cat $^ > $@
	truncate -s 65536 $@

# ── Utility ───────────────────────────────────────────────────────────────────
run: $(build)/os-image.bin
	qemu-system-i386 -drive format=raw,file=$(build)/os-image.bin

clean:
	rm -rf $(build)

.PHONY: all run clean
-include $(shell find $(build) -name '*.d' 2>/dev/null)
