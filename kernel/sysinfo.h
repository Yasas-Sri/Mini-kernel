#ifndef SYSINFO_H
#define SYSINFO_H

#include <stdint.h>
#include <stddef.h>

// System Information Structure
typedef struct
{
    uint64_t total_memory;    // Total memory in bytes
    uint64_t used_memory;     // Used memory in bytes
    uint64_t free_memory;     // Free memory in bytes
    uint32_t interrupt_count; // Number of interrupts handled
    uint64_t uptime_ticks;    // Uptime in timer ticks
    uint32_t cpu_speed_mhz;   // CPU speed in MHz (estimated)
} sysinfo_t;

// BIOS Information
typedef struct
{
    char bios_date[11]; // MM/DD/YYYY
    char bios_version[32];
    uint16_t bios_segment;
    uint8_t boot_device;
} bios_info_t;

// Function Prototypes
void sysinfo_init(void);
void sysinfo_update(void);
void sysinfo_display(void);
void sysinfo_increment_interrupt(uint8_t irq);
void sysinfo_update_uptime(uint32_t ms);
void bios_detect(bios_info_t *info);
void detect_ram(void);
uint32_t estimate_cpu_speed(void);

#endif