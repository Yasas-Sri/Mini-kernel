#include "../drivers/vga.h"
#include "process.h"

// Demo processes for multitasking demonstration

void demo_process_a() {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("[Process A] Starting...\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    for (int i = 0; i < 5; i++) {
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_write_string("[Process A] Iteration ");
        char str[3];
        str[0] = '0' + i;
        str[1] = '\n';
        str[2] = '\0';
        vga_write_string(str);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        
        // Busy wait to simulate work
        for (volatile int j = 0; j < 1000000; j++);
    }
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("[Process A] Exiting\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    process_exit(0);
}

void demo_process_b() {
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_write_string("[Process B] Starting...\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    for (int i = 0; i < 5; i++) {
        vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
        vga_write_string("[Process B] Iteration ");
        char str[3];
        str[0] = '0' + i;
        str[1] = '\n';
        str[2] = '\0';
        vga_write_string(str);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        
        // Busy wait to simulate work
        for (volatile int j = 0; j < 1000000; j++);
    }
    
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_write_string("[Process B] Exiting\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    process_exit(0);
}

void demo_process_c() {
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("[Process C] Starting...\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    for (int i = 0; i < 5; i++) {
        vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        vga_write_string("[Process C] Iteration ");
        char str[3];
        str[0] = '0' + i;
        str[1] = '\n';
        str[2] = '\0';
        vga_write_string(str);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        
        // Busy wait to simulate work
        for (volatile int j = 0; j < 1000000; j++);
    }
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("[Process C] Exiting\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    process_exit(0);
}
