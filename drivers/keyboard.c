#include "keyboard.h"
#include "vga.h"

// Helper function to read from I/O port
static inline uint8_t __inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '};

void keyboard_init()
{
    vga_write_string("[*] Initializing Keyboard Driver...\n");
    // Keyboard initialization will be done via IDT
    vga_write_string("[+] Keyboard initialized\n");
}

char keyboard_getchar()
{
    uint8_t status;
    uint8_t scancode;

    // Wait for keyboard data
    while (1)
    {
        status = __inb(KEYBOARD_STATUS_PORT);
        if (status & 0x01)
        {
            scancode = __inb(KEYBOARD_DATA_PORT);

            // Only handle key press (not release)
            if (scancode < 128)
            {
                if (scancode < sizeof(scancode_to_ascii))
                {
                    char c = scancode_to_ascii[scancode];
                    if (c != 0)
                    {
                        return c;
                    }
                }
            }
        }
    }
}