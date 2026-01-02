#include "rootkit.h"
#include "idt.h"
#include "../drivers/vga.h"


extern struct idt_entry idt_entries[256];


static uint32_t original_keyboard_handler = 0;
static int rootkit_active = 0;


static uint8_t original_shell_init_bytes[32];
static int inline_hook_active = 0;


#define FAKE_MALICIOUS_ADDRESS 0xDEADBEEF

void rootkit_hook_keyboard()
{
    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_write_string("\n[ROOTKIT] Simulating malicious IDT hook...\n");
    
 
    original_keyboard_handler = (idt_entries[39].base_high << 16) | idt_entries[39].base_low;
    
    
    uint32_t fake_handler = FAKE_MALICIOUS_ADDRESS;
    idt_entries[39].base_low = fake_handler & 0xFFFF;
    idt_entries[39].base_high = (fake_handler >> 16) & 0xFFFF;
    
    rootkit_active = 1;
    
    vga_write_string("[ROOTKIT] IRQ7 interrupt hooked (simulated attack)!\n");
    vga_write_string("[ROOTKIT] Original handler: 0x");
    
    
    char addr_str[11];
    const char hex_chars[] = "0123456789ABCDEF";
    addr_str[0] = '0';
    addr_str[1] = 'x';
    for (int i = 7; i >= 0; i--)
    {
        addr_str[2 + (7 - i)] = hex_chars[(original_keyboard_handler >> (i * 4)) & 0xF];
    }
    addr_str[10] = '\0';
    vga_write_string(addr_str);
    vga_write_string("\n");
    
    vga_write_string("[ROOTKIT] Malicious handler: 0x");
    for (int i = 7; i >= 0; i--)
    {
        addr_str[2 + (7 - i)] = hex_chars[(fake_handler >> (i * 4)) & 0xF];
    }
    vga_write_string(addr_str);
    vga_write_string("\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("[!] WARNING: IDT has been modified (detection demo)\n");
    vga_write_string("[!] Note: Using IRQ7 to avoid breaking keyboard\n");
    vga_write_string("[!] Run 'idtcheck' to detect this attack\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void rootkit_unhook()
{
    if (!rootkit_active)
    {
        vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        vga_write_string("\n[ROOTKIT] No active hooks to remove\n\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return;
    }
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("\n[ROOTKIT] Removing malicious hook...\n");
    
  
    idt_entries[39].base_low = original_keyboard_handler & 0xFFFF;
    idt_entries[39].base_high = (original_keyboard_handler >> 16) & 0xFFFF;
    
    rootkit_active = 0;
    
    vga_write_string("[ROOTKIT] IRQ7 handler restored\n");
    vga_write_string("[ROOTKIT] System cleaned\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

int rootkit_is_active()
{
    return rootkit_active;
}

void rootkit_patch_function()
{

    extern void shell_init();
    uint8_t *func_ptr = (uint8_t *)&shell_init;
    
    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    vga_write_string("\n[ROOTKIT] Simulating inline hook (function patching)...\n");
    
  
    for (int i = 0; i < 32; i++)
    {
        original_shell_init_bytes[i] = func_ptr[i];
    }
    

    func_ptr[0] = 0x90;  // NOP
    func_ptr[1] = 0x90;  // NOP
    func_ptr[2] = 0xE9;  // JMP opcode
    func_ptr[3] = 0xEF;  // Fake offset
    func_ptr[4] = 0xBE;
    func_ptr[5] = 0xAD;
    func_ptr[6] = 0xDE;
    
    inline_hook_active = 1;
    
    vga_write_string("[ROOTKIT] Patched shell_init() with fake JMP instruction\n");
    vga_write_string("[ROOTKIT] Modified bytes: 90 90 E9 EF BE AD DE\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("[!] WARNING: Function code has been modified (detection demo)\n");
    vga_write_string("[!] Note: Patched code won't execute - demo only\n");
    vga_write_string("[!] Run 'funccheck' to detect this inline hook\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void rootkit_unpatch_function()
{
    if (!inline_hook_active)
    {
        vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        vga_write_string("\n[ROOTKIT] No inline hooks to remove\n\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return;
    }
    
    extern void shell_init();
    uint8_t *func_ptr = (uint8_t *)&shell_init;
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("\n[ROOTKIT] Removing inline hook...\n");
    
   
    for (int i = 0; i < 32; i++)
    {
        func_ptr[i] = original_shell_init_bytes[i];
    }
    
    inline_hook_active = 0;
    
    vga_write_string("[ROOTKIT] shell_init() code restored\n");
    vga_write_string("[ROOTKIT] Function integrity restored\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

int rootkit_inline_hook_active()
{
    return inline_hook_active;
}
