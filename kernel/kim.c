#include "kim.h"
#include "idt.h"
#include "isr.h"
#include "shell.h"
#include "../drivers/vga.h"
#include <stddef.h>

// External IDT structure from idt.c
extern struct idt_entry idt_entries[256];
extern struct idt_ptr idt_ptr_struct;

// External kernel function for monitoring
extern void kernel_main();

// Baseline IDT snapshot (taken at initialization)
static struct idt_entry idt_baseline[256];
static int kim_initialized = 0;

// Function baseline storage
static function_baseline_t function_baselines[MAX_WATCHED_FUNCTIONS];
static int function_count = 0;

// Helper function to convert hex to string
static void uint32_to_hex(uint32_t value, char *buffer)
{
    const char hex_chars[] = "0123456789ABCDEF";
    buffer[0] = '0';
    buffer[1] = 'x';
    
    for (int i = 7; i >= 0; i--)
    {
        buffer[2 + (7 - i)] = hex_chars[(value >> (i * 4)) & 0xF];
    }
    buffer[10] = '\0';
}

void kim_init()
{
    vga_write_string("[*] Initializing Kernel Integrity Module (KIM)...\n");
    
    // Take baseline snapshot of IDT
    for (int i = 0; i < 256; i++)
    {
        idt_baseline[i] = idt_entries[i];
    }
    
    // Register critical kernel functions for monitoring
    kim_add_function_watch("kernel_main", (uint32_t)&kernel_main);
    kim_add_function_watch("irq_handler", (uint32_t)&irq_handler);
    kim_add_function_watch("fault_handler", (uint32_t)&fault_handler);
    kim_add_function_watch("shell_init", (uint32_t)&shell_init);
    
    kim_initialized = 1;
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] KIM initialized - IDT baseline captured\n");
    vga_write_string("[+] Registered 4 critical functions for integrity monitoring\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

int kim_check_idt()
{
    if (!kim_initialized)
    {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write_string("[!] KIM not initialized!\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return KIM_IDT_COMPROMISED;
    }
    
    int violations = 0;
    
    vga_write_string("\n=== IDT Integrity Check ===\n");
    
    for (int i = 0; i < 256; i++)
    {
        uint32_t handler_addr = (idt_entries[i].base_high << 16) | idt_entries[i].base_low;
        
        // Skip null entries
        if (handler_addr == 0)
            continue;
        
        // Check 1: Handler must be within kernel space
        if (handler_addr < KERNEL_START || handler_addr >= KERNEL_END)
        {
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_write_string("[!] IDT[");
            
            // Simple number to string conversion
            char num_str[4];
            if (i < 10) {
                num_str[0] = '0' + i;
                num_str[1] = '\0';
            } else if (i < 100) {
                num_str[0] = '0' + (i / 10);
                num_str[1] = '0' + (i % 10);
                num_str[2] = '\0';
            } else {
                num_str[0] = '0' + (i / 100);
                num_str[1] = '0' + ((i / 10) % 10);
                num_str[2] = '0' + (i % 10);
                num_str[3] = '\0';
            }
            
            vga_write_string(num_str);
            vga_write_string("] Handler outside kernel space: ");
            
            char addr_str[11];
            uint32_to_hex(handler_addr, addr_str);
            vga_write_string(addr_str);
            vga_write_string("\n");
            
            violations++;
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
        
        // Check 2: Compare with baseline
        uint32_t baseline_addr = (idt_baseline[i].base_high << 16) | idt_baseline[i].base_low;
        
        if (handler_addr != baseline_addr)
        {
            vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
            vga_write_string("[!] IDT[");
            
            char num_str[4];
            if (i < 10) {
                num_str[0] = '0' + i;
                num_str[1] = '\0';
            } else if (i < 100) {
                num_str[0] = '0' + (i / 10);
                num_str[1] = '0' + (i % 10);
                num_str[2] = '\0';
            } else {
                num_str[0] = '0' + (i / 100);
                num_str[1] = '0' + ((i / 10) % 10);
                num_str[2] = '0' + (i % 10);
                num_str[3] = '\0';
            }
            
            vga_write_string(num_str);
            vga_write_string("] Modified! Was: ");
            
            char addr_str[11];
            uint32_to_hex(baseline_addr, addr_str);
            vga_write_string(addr_str);
            vga_write_string(", Now: ");
            uint32_to_hex(handler_addr, addr_str);
            vga_write_string(addr_str);
            vga_write_string("\n");
            
            violations++;
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
    
    if (violations == 0)
    {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write_string("[+] IDT Integrity Check: PASSED\n");
        vga_write_string("[+] All 256 entries verified - No hooks detected\n\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return KIM_OK;
    }
    else
    {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write_string("[!] IDT Integrity Check: FAILED\n");
        vga_write_string("[!] Violations detected: ");
        
        // Print violation count
        char count_str[4];
        if (violations < 10) {
            count_str[0] = '0' + violations;
            count_str[1] = '\0';
        } else {
            count_str[0] = '0' + (violations / 10);
            count_str[1] = '0' + (violations % 10);
            count_str[2] = '\0';
        }
        vga_write_string(count_str);
        vga_write_string("\n\n");
        
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return KIM_IDT_COMPROMISED;
    }
}

void kim_display_idt_info()
{
    vga_write_string("\n=== IDT Information ===\n");
    vga_write_string("IDT Base Address: ");
    
    char addr_str[11];
    uint32_to_hex(idt_ptr_struct.base, addr_str);
    vga_write_string(addr_str);
    vga_write_string("\n");
    
    vga_write_string("IDT Limit: ");
    uint32_to_hex(idt_ptr_struct.limit, addr_str);
    vga_write_string(addr_str);
    vga_write_string("\n");
    
    vga_write_string("Total Entries: 256\n");
    vga_write_string("Kernel Address Range: 0x00010000 - 0x00100000\n\n");
    
    // Show some important handlers
    vga_write_string("Key Interrupt Handlers:\n");
    
    struct {
        int num;
        const char *name;
    } important_isrs[] = {
        {0, "Division Error"},
        {6, "Invalid Opcode"},
        {13, "General Protection"},
        {14, "Page Fault"},
        {32, "Timer (IRQ0)"},
        {33, "Keyboard (IRQ1)"}
    };
    
    for (int i = 0; i < 6; i++)
    {
        uint32_t handler = (idt_entries[important_isrs[i].num].base_high << 16) | 
                           idt_entries[important_isrs[i].num].base_low;
        
        vga_write_string("  ");
        vga_write_string(important_isrs[i].name);
        vga_write_string(": ");
        uint32_to_hex(handler, addr_str);
        vga_write_string(addr_str);
        vga_write_string("\n");
    }
    
    vga_write_string("\n");
}

// CRC32 implementation for function integrity checking
uint32_t kim_crc32(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
    }
    
    return ~crc;
}

void kim_add_function_watch(const char *name, uint32_t address)
{
    if (function_count >= MAX_WATCHED_FUNCTIONS)
    {
        vga_write_string("[!] Cannot watch more functions - limit reached\n");
        return;
    }
    
    // Calculate baseline checksum
    uint8_t *func_ptr = (uint8_t *)address;
    uint32_t checksum = kim_crc32(func_ptr, FUNC_CHECK_SIZE);
    
    function_baselines[function_count].name = name;
    function_baselines[function_count].address = address;
    function_baselines[function_count].checksum = checksum;
    function_baselines[function_count].is_active = 1;
    
    function_count++;
}

int kim_check_functions()
{
    if (!kim_initialized || function_count == 0)
    {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write_string("[!] No functions registered for monitoring!\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return KIM_FUNCTION_COMPROMISED;
    }
    
    int violations = 0;
    
    vga_write_string("\n=== Function Integrity Check ===\n");
    vga_write_string("Checking first 32 bytes of each monitored function...\n\n");
    
    for (int i = 0; i < function_count; i++)
    {
        if (!function_baselines[i].is_active)
            continue;
        
        uint8_t *func_ptr = (uint8_t *)function_baselines[i].address;
        uint32_t current_checksum = kim_crc32(func_ptr, FUNC_CHECK_SIZE);
        
        vga_write_string("  ");
        vga_write_string(function_baselines[i].name);
        vga_write_string(" @ ");
        
        char addr_str[11];
        uint32_to_hex(function_baselines[i].address, addr_str);
        vga_write_string(addr_str);
        vga_write_string(": ");
        
        if (current_checksum != function_baselines[i].checksum)
        {
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_write_string("MODIFIED!\n");
            vga_write_string("    Baseline CRC32: ");
            uint32_to_hex(function_baselines[i].checksum, addr_str);
            vga_write_string(addr_str);
            vga_write_string("\n");
            vga_write_string("    Current CRC32:  ");
            uint32_to_hex(current_checksum, addr_str);
            vga_write_string(addr_str);
            vga_write_string("\n");
            violations++;
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
        else
        {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_write_string("OK\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
    
    vga_write_string("\n");
    
    if (violations == 0)
    {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write_string("[+] Function Integrity Check: PASSED\n");
        vga_write_string("[+] All monitored functions verified - No patches detected\n\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return KIM_OK;
    }
    else
    {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write_string("[!] Function Integrity Check: FAILED\n");
        vga_write_string("[!] Inline hooks detected: ");
        
        char count_str[4];
        if (violations < 10) {
            count_str[0] = '0' + violations;
            count_str[1] = '\0';
        } else {
            count_str[0] = '0' + (violations / 10);
            count_str[1] = '0' + (violations % 10);
            count_str[2] = '\0';
        }
        vga_write_string(count_str);
        vga_write_string(" function(s) modified\n\n");
        
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return KIM_FUNCTION_COMPROMISED;
    }
}
