ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -fno-pic -fno-stack-protector -O0 -mno-sse -mno-mmx -mno-sse2 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

BOOT_SRC = boot/boot.asm
KERNEL_SRC = kernel/kernel.c kernel/gdt.c kernel/idt.c kernel/isr.c kernel/memory.c kernel/shell.c kernel/kim.c kernel/rootkit.c kernel/syscall.c kernel/process.c kernel/demo_processes.c kernel/process_runner.c kernel/string.c kernel/ramdisk.c kernel/vfs.c drivers/vga.c drivers/keyboard.c drivers/pit.c
ASM_SRC = kernel/gdt_flush.asm kernel/idt_flush.asm kernel/isr_stubs.asm kernel/syscall_entry.asm kernel/context_switch.asm

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

kernel/isr_stubs.o: kernel/isr_stubs.asm
	$(ASM) -f elf32 $< -o $@

kernel/syscall_entry.o: kernel/syscall_entry.asm
	$(ASM) -f elf32 $< -o $@

kernel/context_switch.o: kernel/context_switch.asm
	$(ASM) -f elf32 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): kernel/kernel.o kernel/gdt.o kernel/idt.o kernel/isr.o kernel/memory.o kernel/shell.o kernel/kim.o kernel/rootkit.o kernel/syscall.o kernel/process.o kernel/demo_processes.o kernel/process_runner.o kernel/string.o kernel/ramdisk.o kernel/vfs.o drivers/vga.o drivers/keyboard.o drivers/pit.o kernel/gdt_flush.o kernel/idt_flush.o kernel/isr_stubs.o kernel/syscall_entry.o kernel/context_switch.o
	$(LD) $(LDFLAGS) $^ -o kernel.elf
	objcopy -O binary kernel.elf $@

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $@
	# Pad to ensure proper disk image size
	truncate -s 1440K $@

run: $(OS_IMAGE)
	qemu-system-i386 -drive file=$(OS_IMAGE),format=raw,if=floppy -serial stdio

debug: $(OS_IMAGE)
	qemu-system-i386 -drive file=$(OS_IMAGE),format=raw,if=floppy -serial stdio -s -S

clean:
	rm -f *.o kernel/*.o drivers/*.o $(BOOT_BIN) $(KERNEL_BIN) $(OS_IMAGE) kernel.elf

.PHONY: all run debug clean
