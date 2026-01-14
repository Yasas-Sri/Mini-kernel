#include "vga.h"

static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY;
static uint8_t vga_cursor_x = 0;
static uint8_t vga_cursor_y = 0;
static uint8_t vga_color_code = 0x0F;
static int current_theme = 0; // 0 = dark, 1 = light 

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

void vga_init()
{
    vga_color_code = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_clear()
{
    // Use current theme colors
    if (current_theme == 1) {
        vga_color_code = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
    } else {
        vga_color_code = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
    
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            const int index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color_code);
        }
    }
    vga_cursor_x = 0;
    vga_cursor_y = 0;
}

void vga_scroll()
{
   
    for (int y = 0; y < VGA_HEIGHT - 1; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }

    
    for (int x = 0; x < VGA_WIDTH; x++)
    {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', vga_color_code);
    }

    vga_cursor_y = VGA_HEIGHT - 1;
}

void vga_putchar(char c)
{
    if (c == '\n')
    {
        vga_cursor_x = 0;
        vga_cursor_y++;
    }
    else if (c == '\r')
    {
        vga_cursor_x = 0;
    }
    else if (c == '\t')
    {
        vga_cursor_x = (vga_cursor_x + 4) & ~(4 - 1);
    }
    else if (c == '\b')
    {
        if (vga_cursor_x > 0)
        {
            vga_cursor_x--;
            const int index = vga_cursor_y * VGA_WIDTH + vga_cursor_x;
            vga_buffer[index] = vga_entry(' ', vga_color_code);
        }
    }
    else
    {
        const int index = vga_cursor_y * VGA_WIDTH + vga_cursor_x;
        vga_buffer[index] = vga_entry(c, vga_color_code);
        vga_cursor_x++;
    }

    if (vga_cursor_x >= VGA_WIDTH)
    {
        vga_cursor_x = 0;
        vga_cursor_y++;
    }

    if (vga_cursor_y >= VGA_HEIGHT)
    {
        vga_scroll();
    }
}

void vga_write_string(const char *str)
{
    while (*str)
    {
        vga_putchar(*str++);
    }
}

void vga_set_color(uint8_t fg, uint8_t bg)
{
    vga_color_code = vga_entry_color(fg, bg);
}

void vga_write_dec(int num)
{
    if (num == 0) {
        vga_putchar('0');
        return;
    }
    if (num < 0) {
        vga_putchar('-');
        num = -num;
    }
    char buffer[12];
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0) {
        vga_putchar(buffer[--i]);
    }
}

void vga_apply_dark_theme()
{
    current_theme = 0;
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    // Redraw screen with new colors
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            uint16_t entry = vga_buffer[index];
            unsigned char ch = entry & 0xFF;
            vga_buffer[index] = vga_entry(ch, vga_color_code);
        }
    }
}

void vga_apply_light_theme()
{
    current_theme = 1;
    vga_set_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
    // Fill entire screen with light theme colors
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            uint16_t entry = vga_buffer[index];
            unsigned char ch = entry & 0xFF;
            // If it's a space or any character, apply new theme
            vga_buffer[index] = vga_entry(ch, vga_color_code);
        }
    }
}

void vga_get_theme_colors(uint8_t *fg, uint8_t *bg)
{
    if (current_theme == 1) {
        // Light theme
        *fg = VGA_COLOR_BLACK;
        *bg = VGA_COLOR_WHITE;
    } else {
        // Dark theme
        *fg = VGA_COLOR_LIGHT_GREY;
        *bg = VGA_COLOR_BLACK;
    }
}