# Project root is one directory above this makefile, regardless of where
# `make` is invoked from.
ROOT  := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
BUILD := $(ROOT)/build

CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

CFLAGS = -ffreestanding -m32 -nostdlib -nostartfiles -nodefaultlibs -I$(ROOT)/kernel -I$(ROOT)/kernel/arch/x86 -I$(ROOT)/drivers -I$(ROOT)/lib -MMD -MP
LDFLAGS = -T $(ROOT)/make/linker.ld --oformat binary

all: $(BUILD)/os-image.bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.bin: $(ROOT)/bootloader/bootloader.asm $(ROOT)/bootloader/print.asm $(ROOT)/bootloader/disk.asm | $(BUILD)
	$(AS) -f bin -i $(ROOT)/bootloader/ $(ROOT)/bootloader/bootloader.asm -o $@

$(BUILD)/kernel.o: $(ROOT)/kernel/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/idt.o: $(ROOT)/kernel/arch/x86/idt.c $(ROOT)/kernel/arch/x86/idt.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/helper.o: $(ROOT)/lib/helper.c $(ROOT)/lib/helper.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/keyboard.o: $(ROOT)/drivers/keyboard.c $(ROOT)/drivers/keyboard.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/entry.o: $(ROOT)/kernel/entry.asm | $(BUILD)
	$(AS) -f elf $< -o $@

$(BUILD)/idt_asm.o: $(ROOT)/kernel/arch/x86/idt.asm $(ROOT)/kernel/arch/x86/pic.h | $(BUILD)
	$(AS) -f elf $< -o $@

$(BUILD)/pic_asm.o: $(ROOT)/kernel/arch/x86/pic.asm | $(BUILD)
	$(AS) -f elf $< -o $@

$(BUILD)/pic.o: $(ROOT)/kernel/arch/x86/pic.c $(ROOT)/kernel/arch/x86/pic.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/pit.o: $(ROOT)/kernel/arch/x86/pit.c $(ROOT)/kernel/arch/x86/pit.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.bin: $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/helper.o $(BUILD)/keyboard.o $(BUILD)/idt_asm.o $(BUILD)/pic_asm.o $(BUILD)/pic.o $(BUILD)/pit.o $(ROOT)/make/linker.ld | $(BUILD)
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/helper.o $(BUILD)/keyboard.o  $(BUILD)/idt_asm.o $(BUILD)/pic_asm.o $(BUILD)/pic.o $(BUILD)/pit.o

$(BUILD)/os-image.bin: $(BUILD)/boot.bin $(BUILD)/kernel.bin | $(BUILD)
	cat $(BUILD)/boot.bin $(BUILD)/kernel.bin > $@
	# Pad image to 64 KB so BIOS int 13h can read past the end of the kernel
	truncate -s 65536 $@

run: $(BUILD)/os-image.bin
	qemu-system-i386 -drive format=raw,file=$(BUILD)/os-image.bin

clean:
	rm -rf $(BUILD)

.PHONY: all run clean

-include $(BUILD)/*.d