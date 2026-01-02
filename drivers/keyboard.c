#include "keyboard.h"
#include "vga.h"


static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int buffer_read_pos = 0;
static volatile int buffer_write_pos = 0;


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
    vga_write_string("[*] Initializing Keyboard Driver (Interrupt Mode)...\n");
    buffer_read_pos = 0;
    buffer_write_pos = 0;
    vga_write_string("[+] Keyboard initialized\n");
}

void keyboard_handler()
{
    uint8_t scancode = __inb(KEYBOARD_DATA_PORT);
    
    
    if (scancode < 128)
    {
        if (scancode < sizeof(scancode_to_ascii))
        {
            char c = scancode_to_ascii[scancode];
            if (c != 0)
            {
                
                int next_pos = (buffer_write_pos + 1) % KEYBOARD_BUFFER_SIZE;
                if (next_pos != buffer_read_pos)
                {
                    keyboard_buffer[buffer_write_pos] = c;
                    buffer_write_pos = next_pos;
                }
            }
        }
    }
}

char keyboard_getchar()
{
   
    while (buffer_read_pos == buffer_write_pos)
    {
        
        __asm__ volatile("hlt");
    }
    
    char c = keyboard_buffer[buffer_read_pos];
    buffer_read_pos = (buffer_read_pos + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}