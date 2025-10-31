#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "gdt.h"
#include "idt.h"
#include "memory.h"
#include "shell.h"

void kernel_main()
{
    // Initialize VGA driver
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

    // Initialize Memory Manager
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    memory_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] Memory manager initialized\n\n");

    // Initialize Keyboard
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    keyboard_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("\n");

    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("Kernel initialized. System ready.\n");

    // Start shell
    shell_init();
    shell_run();

    // Should never reach here
    while (1)
    {
        __asm__ volatile("hlt");
    }
}