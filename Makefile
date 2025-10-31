ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -O2 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

BOOT_SRC = boot/boot.asm
KERNEL_SRC = kernel/kernel.c kernel/gdt.c kernel/idt.c kernel/memory.c kernel/shell.c drivers/vga.c drivers/keyboard.c
ASM_SRC = kernel/gdt_flush.asm kernel/idt_flush.asm

BOOT_BIN = boot.bin
KERNEL_BIN = kernel.bin
OS_IMAGE = MiniKernelSec.bin

all: $(OS_IMAGE)

$(BOOT_BIN): $(BOOT_SRC)
	$(ASM) -f bin $< -o $@

kernel/gdt_flush.o: kernel/gdt_flush.asm
	$(ASM) -f elf32 $< -o $@

kernel/idt_flush.o: kernel/idt_flush.asm
	$(ASM) -f elf32 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): kernel/kernel.o kernel/gdt.o kernel/idt.o kernel/memory.o kernel/shell.o drivers/vga.o drivers/keyboard.o kernel/gdt_flush.o kernel/idt_flush.o
	$(LD) $(LDFLAGS) $^ -o kernel.elf
	objcopy -O binary kernel.elf $@

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $@
	# Pad to ensure proper disk image size
	truncate -s 1440K $@

run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -serial stdio

debug: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -serial stdio -s -S

clean:
	rm -f *.o kernel/*.o drivers/*.o $(BOOT_BIN) $(KERNEL_BIN) $(OS_IMAGE) kernel.elf

.PHONY: all run debug clean
