# Project root is one directory above this makefile, regardless of where
# `make` is invoked from.
ROOT  := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
BUILD := $(ROOT)/build

CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

CFLAGS  = -ffreestanding -m32 -nostdlib -nostartfiles -nodefaultlibs -I$(ROOT)/kernel
LDFLAGS = -T $(ROOT)/make/linker.ld --oformat binary

all: $(BUILD)/os-image.bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.bin: $(ROOT)/bootloader/bootloader.asm $(ROOT)/bootloader/print.asm $(ROOT)/bootloader/disk.asm | $(BUILD)
	$(AS) -f bin -i $(ROOT)/bootloader/ $(ROOT)/bootloader/bootloader.asm -o $@

$(BUILD)/kernel.o: $(ROOT)/kernel/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/idt.o: $(ROOT)/kernel/idt.c $(ROOT)/kernel/idt.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/entry.o: $(ROOT)/kernel/entry.asm | $(BUILD)
	$(AS) -f elf $< -o $@

$(BUILD)/idt_asm.o: $(ROOT)/kernel/idt.asm | $(BUILD)
	$(AS) -f elf $< -o $@

$(BUILD)/kernel.bin: $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/idt_asm.o $(ROOT)/make/linker.ld | $(BUILD)
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/idt_asm.o

$(BUILD)/os-image.bin: $(BUILD)/boot.bin $(BUILD)/kernel.bin | $(BUILD)
	cat $(BUILD)/boot.bin $(BUILD)/kernel.bin > $@
	# Pad image to 64 KB so BIOS int 13h can read past the end of the kernel
	truncate -s 65536 $@

run: $(BUILD)/os-image.bin
	qemu-system-i386 -drive format=raw,file=$(BUILD)/os-image.bin

clean:
	rm -rf $(BUILD)

.PHONY: all run clean
