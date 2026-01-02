#include "shell.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "memory.h"
#include "kim.h"
#include "rootkit.h"
#include <stdint.h>

#define MAX_INPUT 256

static char command_buffer[MAX_INPUT];
static int buffer_pos = 0;

// String comparison
static int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// String copy
static void strcpy(char *dest, const char *src)
{
    while ((*dest++ = *src++))
        ;
}

// String length
static int strlen(const char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return len;
}

void shell_init()
{
    vga_write_string("\n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("   MiniKernel-Sec Shell v0.1\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("Type 'help' for available commands\n\n");
}

static void print_prompt()
{
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("kernel> ");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void execute_command(const char *cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        vga_write_string("\nAvailable commands:\n");
        vga_write_string("  help      - Show this help message\n");
        vga_write_string("  clear     - Clear the screen\n");
        vga_write_string("  meminfo   - Display memory information\n");
        vga_write_string("  about     - Show kernel information\n");
        vga_write_string("  test      - Test memory allocator\n");
        vga_write_string("  idtcheck  - Check IDT integrity\n");
        vga_write_string("  idtinfo   - Display IDT information\n");
        vga_write_string("  funccheck - Check function integrity (CRC32)\n");
        vga_write_string("  attack    - Simulate rootkit IDT hook attack\n");
        vga_write_string("  unhook    - Remove rootkit IDT hook\n");
        vga_write_string("  patch     - Simulate inline hook (function patching)\n");
        vga_write_string("  unpatch   - Remove inline hook\n");
        vga_write_string("\n");
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        vga_clear();
    }
    else if (strcmp(cmd, "meminfo") == 0)
    {
        memory_info();
    }
    else if (strcmp(cmd, "about") == 0)
    {
        vga_write_string("\nMiniKernel-Sec v0.1\n");
        vga_write_string("A minimalist kernel with security features\n");
        vga_write_string("Built for educational purposes\n\n");
    }
    else if (strcmp(cmd, "test") == 0)
    {
        vga_write_string("\n[*] Testing memory allocator...\n");

        void *ptr1 = kmalloc(100);
        vga_write_string("[+] Allocated 100 bytes at: 0x");
        vga_write_string("...\n");

        void *ptr2 = kmalloc(200);
        vga_write_string("[+] Allocated 200 bytes at: 0x");
        vga_write_string("...\n");

        kfree(ptr1);
        vga_write_string("[+] Freed first allocation\n");

        void *ptr3 = kmalloc(50);
        vga_write_string("[+] Allocated 50 bytes at: 0x");
        vga_write_string("...\n");

        kfree(ptr2);
        kfree(ptr3);
        vga_write_string("[+] All memory freed\n");
        vga_write_string("[+] Memory allocator test complete!\n\n");
    }
    else if (strcmp(cmd, "idtcheck") == 0)
    {
        kim_check_idt();
    }
    else if (strcmp(cmd, "idtinfo") == 0)
    {
        kim_display_idt_info();
    }
    else if (strcmp(cmd, "funccheck") == 0)
    {
        kim_check_functions();
    }
    else if (strcmp(cmd, "attack") == 0)
    {
        rootkit_hook_keyboard();
    }
    else if (strcmp(cmd, "unhook") == 0)
    {
        rootkit_unhook();
    }
    else if (strcmp(cmd, "patch") == 0)
    {
        rootkit_patch_function();
    }
    else if (strcmp(cmd, "unpatch") == 0)
    {
        rootkit_unpatch_function();
    }
    else if (strlen(cmd) > 0)
    {
        vga_write_string("Unknown command: ");
        vga_write_string(cmd);
        vga_write_string("\nType 'help' for available commands\n");
    }
}

void shell_run()
{
    buffer_pos = 0;

    while (1)
    {
        print_prompt();

        // Read command
        while (1)
        {
            char c = keyboard_getchar();

            if (c == '\n')
            {
                vga_putchar('\n');
                command_buffer[buffer_pos] = '\0';
                execute_command(command_buffer);
                buffer_pos = 0;
                break;
            }
            else if (c == '\b')
            {
                if (buffer_pos > 0)
                {
                    buffer_pos--;
                    vga_putchar('\b');
                }
            }
            else if (buffer_pos < MAX_INPUT - 1)
            {
                command_buffer[buffer_pos++] = c;
                vga_putchar(c);
            }
        }
    }
}