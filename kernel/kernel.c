#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/pit.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "memory.h"
#include "shell.h"
#include "kim.h"
#include "syscall.h"
#include "process.h"
#include "vfs.h"
#include "sysinfo.h"
#include "pci.h"

void kernel_main()
{
   
    vga_init();

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("MiniKernel-Sec v0.1\n");
    vga_write_string("====================\n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("[*] Initializing GDT...\n");
    gdt_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] GDT initialized successfully\n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("[*] Initializing IDT...\n");
    idt_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] IDT initialized successfully\n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("[*] Installing ISR handlers...\n");
    isr_install();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] Exception handlers installed\n\n");

    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("[*] Installing IRQ handlers...\n");
    irq_install();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] Hardware interrupt handlers installed\n\n");

    // Initialize system calls
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    syscall_init();
    vga_write_string("\n");

    // Initialize process management
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    process_init();
    vga_write_string("\n");

    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    kim_init();
    vga_write_string("\n");

   
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    memory_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] Memory manager initialized\n\n");

    // Initialize VFS and RAM disk
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("[*] Initializing Virtual File System...\n");
    vfs_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] VFS initialized\n");
    vfs_load_config();
    vga_write_string("\n");

    // Initialize system information
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("[*] Gathering system information...\n");
    sysinfo_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] System information initialized\n\n");

    // Initialize PCI
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    pci_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] PCI initialized\n\n");

   
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    keyboard_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("\n");

    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    pit_init(100); 
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("\n");

    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("Kernel initialized. System ready.\n");

   
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("[*] Enabling hardware interrupts...\n");
    __asm__ volatile("sti");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] Interrupts enabled\n\n");

  
    shell_init();
    shell_run();

    
    while (1)
    {
        __asm__ volatile("hlt");
    }
}