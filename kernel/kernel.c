#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/pit.h"
#include "../drivers/serial.h"
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
#include "rootkit_detect.h"
#include "banner.h"
#include "prompt.h"
#include "ui_widgets.h"

void kernel_main()
{
   
    // Initialize serial port first for early debugging
    serial_init();
    
    vga_init();
    
    // Show boot banner
    display_boot_banner();
    display_feature_summary();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("\n  BOOT SEQUENCE STARTED\n");
    ui_separator('─');
    vga_write_string("\n");

    ui_status_info("Initializing GDT...");
    gdt_init();
    ui_status_success("GDT initialized successfully");

    ui_status_info("Initializing IDT...");
    idt_init();
    ui_status_success("IDT initialized successfully");

    ui_status_info("Installing ISR handlers...");
    isr_install();
    ui_status_success("Exception handlers installed");

    ui_status_info("Installing IRQ handlers...");
    irq_install();
    ui_status_success("Hardware interrupt handlers installed");

    ui_status_info("Initializing system calls...");
    syscall_init();
    ui_status_success("System call interface ready");

    ui_status_info("Initializing process management...");
    process_init();
    ui_status_success("Process scheduler ready");

    ui_status_info("Initializing KIM (Kernel Integrity Monitor)...");
    kim_init();
    ui_status_success("KIM initialized");

    ui_status_info("Initializing memory manager...");
    memory_init();
    ui_status_success("Memory manager initialized");

    ui_status_info("Initializing Virtual File System...");
    vfs_init();
    vfs_load_config();
    ui_status_success("VFS initialized with RAM disk");

    ui_status_info("Gathering system information...");
    sysinfo_init();
    ui_status_success("System information initialized");

    ui_status_info("Scanning PCI bus...");
    pci_init();
    ui_status_success("PCI enumeration complete");

    ui_status_info("Initializing rootkit detection...");
    rootkit_detect_init();
    ui_status_success("Rootkit detection baseline established");

    ui_status_info("Initializing keyboard driver...");
    keyboard_init();
    ui_status_success("Keyboard driver ready");

    ui_status_info("Initializing PIT (100 Hz)...");
    pit_init(100); 
    ui_status_success("Programmable Interval Timer configured");

    vga_write_string("\n");
    ui_separator('═');
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("\n  ✓ ALL SYSTEMS OPERATIONAL\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    ui_status_info("Enabling hardware interrupts...");
    __asm__ volatile("sti");
    ui_status_success("Interrupts enabled");
    
    vga_write_string("\n");
    ui_separator('─');
    vga_write_string("\n");
    
    // Initialize prompt system
    prompt_init();
    
    shell_init();
    shell_run();

    
    while (1)
    {
        __asm__ volatile("hlt");
    }
}