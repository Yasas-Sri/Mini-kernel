#include "prompt.h"
#include "../drivers/vga.h"

static uint8_t current_style = PROMPT_STYLE_COLORFUL;
static char current_user[32] = "root";
static char current_path[64] = "/";

// String copy helper
static void strcpy_local(char *dest, const char *src, int max_len) {
    int i = 0;
    while (src[i] && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void prompt_init() {
    current_style = PROMPT_STYLE_COLORFUL;
    strcpy_local(current_user, "root", sizeof(current_user));
    strcpy_local(current_path, "/", sizeof(current_path));
}

void prompt_set_style(uint8_t style) {
    current_style = style;
}

void prompt_set_user(const char* username) {
    if (username) {
        strcpy_local(current_user, username, sizeof(current_user));
    }
}

void prompt_set_path(const char* path) {
    if (path) {
        strcpy_local(current_path, path, sizeof(current_path));
    }
}

void prompt_print() {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    switch (current_style) {
        case PROMPT_STYLE_SIMPLE:
            vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
            vga_write_string("> ");
            vga_set_color(fg, bg);
            break;
            
        case PROMPT_STYLE_COLORFUL:
            // [root@minikernel /]#
            vga_set_color(VGA_COLOR_DARK_GREY, bg);
            vga_write_string("[");
            
            vga_set_color(VGA_COLOR_LIGHT_RED, bg);
            vga_write_string(current_user);
            
            vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
            vga_write_string("@");
            
            vga_set_color(VGA_COLOR_LIGHT_CYAN, bg);
            vga_write_string("minikernel");
            
            vga_set_color(VGA_COLOR_DARK_GREY, bg);
            vga_write_string(" ");
            
            vga_set_color(VGA_COLOR_LIGHT_BLUE, bg);
            vga_write_string(current_path);
            
            vga_set_color(VGA_COLOR_DARK_GREY, bg);
            vga_write_string("]");
            
            vga_set_color(VGA_COLOR_LIGHT_RED, bg);
            vga_write_string("# ");
            
            vga_set_color(fg, bg);
            break;
            
        case PROMPT_STYLE_MINIMAL:
            vga_set_color(VGA_COLOR_LIGHT_BLUE, bg);
            vga_write_string(current_path);
            vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
            vga_write_string(" $ ");
            vga_set_color(fg, bg);
            break;
            
        case PROMPT_STYLE_HACKER:
            // >>> [SYSTEM READY]
            vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
            vga_write_string(">>> ");
            
            vga_set_color(VGA_COLOR_DARK_GREY, bg);
            vga_write_string("[");
            
            vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
            vga_write_string("SYSTEM READY");
            
            vga_set_color(VGA_COLOR_DARK_GREY, bg);
            vga_write_string("] ");
            
            vga_set_color(fg, bg);
            break;
            
        default:
            vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
            vga_write_string("kernel> ");
            vga_set_color(fg, bg);
            break;
    }
}
