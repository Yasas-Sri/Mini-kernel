#include "../drivers/vga.h"

void display_boot_banner() {
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
    vga_write_string("\n");
    vga_write_string("  ███╗   ███╗██╗███╗   ██╗██╗    ██╗  ██╗███████╗██████╗ ███╗   ██╗███████╗██╗     \n");
    vga_write_string("  ████╗ ████║██║████╗  ██║██║    ██║ ██╔╝██╔════╝██╔══██╗████╗  ██║██╔════╝██║     \n");
    vga_write_string("  ██╔████╔██║██║██╔██╗ ██║██║    █████╔╝ █████╗  ██████╔╝██╔██╗ ██║█████╗  ██║     \n");
    vga_write_string("  ██║╚██╔╝██║██║██║╚██╗██║██║    ██╔═██╗ ██╔══╝  ██╔══██╗██║╚██╗██║██╔══╝  ██║     \n");
    vga_write_string("  ██║ ╚═╝ ██║██║██║ ╚████║██║    ██║  ██╗███████╗██║  ██║██║ ╚████║███████╗███████╗\n");
    vga_write_string("  ╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝    ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("\n                        -= Security-Enhanced Operating System =-\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("                              Version 1.0 | Build 2026\n");
    
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_write_string("                    ─────────────────────────────────────────────\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void display_feature_summary() {
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("  ✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("Protected Mode (32-bit)       ");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("Preemptive Multitasking\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("  ✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("RAM Filesystem (16 files)     ");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("System Call Interface\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("  ✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("Rootkit Detection Engine      ");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("IDT/Function Integrity\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("  ✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("Dynamic Memory Allocator      ");
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓ ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("Hardware Introspection\n\n");
    
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_write_string("                    ─────────────────────────────────────────────\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}
