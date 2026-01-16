#include "pager.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"

#define LINES_PER_PAGE 20

static int strlen_pager(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void pager_display(const char* text) {
    int line_count = 0;
    int i = 0;
    int page_lines = 0;
    
    while (text[i]) {
        // Display character
        vga_putchar(text[i]);
        
        // Count newlines
        if (text[i] == '\n') {
            line_count++;
            page_lines++;
            
            // Pause after LINES_PER_PAGE lines
            if (page_lines >= LINES_PER_PAGE) {
                vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
                vga_write_string("\n--- Press SPACE for more, Q to quit ---");
                
                // Wait for keypress
                char key = keyboard_getchar();
                
                // Clear the prompt line
                vga_write_string("\r                                           \r");
                
                uint8_t fg, bg;
                extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
                vga_get_theme_colors(&fg, &bg);
                vga_set_color(fg, bg);
                
                if (key == 'q' || key == 'Q') {
                    vga_write_string("\n");
                    return;
                }
                
                page_lines = 0;
            }
        }
        
        i++;
    }
    
    vga_write_string("\n");
}

void pager_display_lines(const char** lines, int num_lines) {
    int page_lines = 0;
    
    for (int i = 0; i < num_lines; i++) {
        vga_write_string(lines[i]);
        vga_write_string("\n");
        page_lines++;
        
        // Pause after LINES_PER_PAGE lines
        if (page_lines >= LINES_PER_PAGE && i < num_lines - 1) {
            vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
            vga_write_string("--- Press SPACE for more, Q to quit ---");
            
            // Wait for keypress
            char key = keyboard_getchar();
            
            // Clear the prompt line
            vga_write_string("\r                                           \r");
            
            uint8_t fg, bg;
            extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
            vga_get_theme_colors(&fg, &bg);
            vga_set_color(fg, bg);
            
            if (key == 'q' || key == 'Q') {
                vga_write_string("\n");
                return;
            }
            
            page_lines = 0;
        }
    }
}
