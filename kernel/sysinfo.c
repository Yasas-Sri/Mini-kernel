#include "sysinfo.h"
#include "../drivers/vga.h"
#include "memory.h"
#include "idt.h"
#include "cpu_info.h"
#include <stdint.h>

// Global system information
static sysinfo_t sysinfo = {0};
static bios_info_t bios_info = {0};

// Initialize system information
void sysinfo_init(void)
{
    sysinfo.total_memory = 0;
    sysinfo.used_memory = 0;
    sysinfo.free_memory = 0;
    sysinfo.interrupt_count = 0;
    sysinfo.uptime_ticks = 0;
    sysinfo.cpu_speed_mhz = 0;

    // Detect BIOS information
    bios_detect(&bios_info);

    // Detect RAM
    detect_ram();
    
    // Calculate free memory (initially all memory is free)
    sysinfo.free_memory = sysinfo.total_memory;

    // Estimate CPU speed
    sysinfo.cpu_speed_mhz = estimate_cpu_speed();
}

// Update system information (call periodically)
void sysinfo_update(void)
{
    // Interrupt count is now tracked internally via sysinfo_increment_interrupt()

    // Update memory usage (you need to implement memory_get_stats() in memory.c)
    // For now, we'll use a placeholder
    sysinfo.used_memory = 0; // Placeholder
    sysinfo.free_memory = sysinfo.total_memory - sysinfo.used_memory;

    // Update uptime (we need to implement a timer for this)
    sysinfo.uptime_ticks++; // This should be updated by timer interrupt
}

// Display system information
void sysinfo_display(void)
{
    vga_write_string("\nSystem Information\n");
    vga_write_string("==================\n\n");

    
    cpu_print_info();
    vga_write_string("  Estimated Speed: ");
    vga_write_dec(sysinfo.cpu_speed_mhz);
    vga_write_string(" MHz\n\n");

    // Memory Information (use 32-bit division)
    vga_write_string("Memory:\n");
    vga_write_string("  Total: ");
    uint32_t total_kb = (uint32_t)((sysinfo.total_memory >> 10)); // Shift instead of divide
    vga_write_dec(total_kb);
    vga_write_string(" KB\n");
    vga_write_string("  Used: ");
    uint32_t used_kb = (uint32_t)((sysinfo.used_memory >> 10));
    vga_write_dec(used_kb);
    vga_write_string(" KB\n");
    vga_write_string("  Free: ");
    uint32_t free_kb = (uint32_t)((sysinfo.free_memory >> 10));
    vga_write_dec(free_kb);
    vga_write_string(" KB\n\n");

    // System Stats
    vga_write_string("System Stats:\n");
    vga_write_string("  Interrupts Handled: ");
    vga_write_dec(sysinfo.interrupt_count);
    vga_write_string("\n");

    // Calculate uptime in seconds (using 32-bit arithmetic)
    uint32_t uptime_ms = (uint32_t)(sysinfo.uptime_ticks); // Already in milliseconds
    uint32_t uptime_seconds = uptime_ms / 1000;
    vga_write_string("  Uptime: ");
    vga_write_dec(uptime_seconds / 3600); // Hours
    vga_write_string("h ");
    vga_write_dec((uptime_seconds % 3600) / 60); // Minutes
    vga_write_string("m ");
    vga_write_dec(uptime_seconds % 60); // Seconds
    vga_write_string("s\n\n");

    // BIOS Information
    vga_write_string("BIOS Information:\n");
    vga_write_string("  Date: ");
    vga_write_string(bios_info.bios_date);
    vga_write_string("\n");
    vga_write_string("  Boot Device: 0x");
    vga_write_hex(bios_info.boot_device);
    vga_write_string("\n");
}

// Increment interrupt counter
void sysinfo_increment_interrupt(uint8_t irq)
{
    (void)irq; // Unused for now
    sysinfo.interrupt_count++;
}

// Update uptime
void sysinfo_update_uptime(uint32_t ms)
{
    sysinfo.uptime_ticks += ms;
}

// Detect BIOS information
void bios_detect(bios_info_t *info)
{
    // BIOS date is typically at F000:FFF5 (0xFFFF5)
    char *bios_date_ptr = (char *)0xFFFF5;
    for (int i = 0; i < 10; i++)
    {
        info->bios_date[i] = bios_date_ptr[i];
    }
    info->bios_date[10] = '\0';

    // Simple BIOS version detection (placeholder)
    const char *version = "Unknown";
    for (int i = 0; i < 31 && version[i]; i++)
    {
        info->bios_version[i] = version[i];
    }
    info->bios_version[31] = '\0';

    // BIOS segment
    info->bios_segment = 0xF000;

    // Boot device (this should be passed by bootloader)
    // For now, assume floppy (0x00)
    info->boot_device = 0x00;
}

// Detect RAM (simple detection using BIOS INT 15h E820 via bootloader)
void detect_ram(void)
{
    // This is a simplified version
    // In a real system, you'd get this from bootloader
    // For now, we'll assume 128MB (common for educational kernels)
    sysinfo.total_memory = 128 * 1024 * 1024; // 128 MB
}

// Estimate CPU speed using PIT
uint32_t estimate_cpu_speed(void)
{
    // This is a simplified estimation
    // In reality, you'd calibrate against PIT
    return 1000; // Assume 1 GHz for now
}