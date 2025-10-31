#include "vga.h"

static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY;
static uint8_t vga_cursor_x = 0;
static uint8_t vga_cursor_y = 0;
static uint8_t vga_color_code = 0x0F; // White on black

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
    // Move all lines up
    for (int y = 0; y < VGA_HEIGHT - 1; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }

    // Clear last line
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