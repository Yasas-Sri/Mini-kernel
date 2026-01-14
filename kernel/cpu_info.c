#include "cpu_info.h"
#include "../drivers/vga.h"
#include <stdint.h>

// Inline assembly for CPUID instruction
static inline void cpuid(uint32_t code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    asm volatile("cpuid"
                 : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                 : "a"(code));
}

// Get CPU Vendor
void cpu_detect(cpu_info_t *info)
{
    uint32_t eax, ebx, ecx, edx;

    // Get vendor string
    cpuid(0, &eax, &ebx, &ecx, &edx);
    info->max_cpuid = eax;

    // Store vendor string (ebx, edx, ecx in that order)
    *((uint32_t *)&info->vendor[0]) = ebx;
    *((uint32_t *)&info->vendor[4]) = edx;
    *((uint32_t *)&info->vendor[8]) = ecx;
    info->vendor[12] = '\0';

    // Get processor info
    cpuid(1, &eax, &ebx, &ecx, &edx);

    // Extract family, model, stepping
    info->stepping = eax & 0xF;
    info->model = (eax >> 4) & 0xF;
    info->family = (eax >> 8) & 0xF;

    // If family is 0xF, need to add extended family
    if (info->family == 0xF)
    {
        info->family += (eax >> 20) & 0xFF;
    }

    // If model is 0xF, need to add extended model
    if (info->family == 0x6 || info->family == 0xF)
    {
        info->model |= ((eax >> 16) & 0xF) << 4;
    }

    // Extract feature flags
    info->features.has_fpu = (edx >> 0) & 1;
    info->features.has_vme = (edx >> 1) & 1;
    info->features.has_de = (edx >> 2) & 1;
    info->features.has_pse = (edx >> 3) & 1;
    info->features.has_tsc = (edx >> 4) & 1;
    info->features.has_msr = (edx >> 5) & 1;
    info->features.has_pae = (edx >> 6) & 1;
    info->features.has_mce = (edx >> 7) & 1;
    info->features.has_cx8 = (edx >> 8) & 1;
    info->features.has_apic = (edx >> 9) & 1;
    info->features.has_mmx = (edx >> 23) & 1;
    info->features.has_sse = (edx >> 25) & 1;
    info->features.has_sse2 = (edx >> 26) & 1;

    // Try to get brand string if supported
    // First check if extended CPUID is available
    uint32_t max_extended;
    cpuid(0x80000000, &max_extended, &ebx, &ecx, &edx);
    if (max_extended >= 0x80000004)
    {
        char *brand = info->brand;
        cpuid(0x80000002, (uint32_t *)&brand[0], (uint32_t *)&brand[4],
              (uint32_t *)&brand[8], (uint32_t *)&brand[12]);
        cpuid(0x80000003, (uint32_t *)&brand[16], (uint32_t *)&brand[20],
              (uint32_t *)&brand[24], (uint32_t *)&brand[28]);
        cpuid(0x80000004, (uint32_t *)&brand[32], (uint32_t *)&brand[36],
              (uint32_t *)&brand[40], (uint32_t *)&brand[44]);
        brand[48] = '\0';
    }
    else
    {
        // Simple identification based on family/model
        const char *brand = "Unknown x86 CPU";
        for (int i = 0; i < 48 && brand[i]; i++)
        {
            info->brand[i] = brand[i];
        }
    }
}

// Print CPU information
void cpu_print_info(void)
{
    cpu_info_t info;
    cpu_detect(&info);

    vga_write_string("CPU Information:\n");
    vga_write_string("===============\n");
    vga_write_string("Vendor: ");
    vga_write_string(info.vendor);
    vga_write_string("\n");

    vga_write_string("Brand: ");
    vga_write_string(info.brand);
    vga_write_string("\n");

    vga_write_string("Family: 0x");
    vga_write_hex(info.family);
    vga_write_string("  Model: 0x");
    vga_write_hex(info.model);
    vga_write_string("  Stepping: 0x");
    vga_write_hex(info.stepping);
    vga_write_string("\n");

    vga_write_string("Max CPUID: 0x");
    vga_write_hex(info.max_cpuid);
    vga_write_string("\n");
}

// Get CPU vendor string
const char *cpu_get_vendor(void)
{
    static cpu_info_t info;
    cpu_detect(&info);
    return info.vendor;
}

// Print CPU features
void cpu_print_features(void)
{
    cpu_info_t info;
    cpu_detect(&info);

    vga_write_string("CPU Features:\n");
    vga_write_string("=============\n");

    vga_write_string(info.features.has_fpu ? "[X] FPU\n" : "[ ] FPU\n");
    vga_write_string(info.features.has_vme ? "[X] VME\n" : "[ ] VME\n");
    vga_write_string(info.features.has_de ? "[X] Debugging Extensions\n" : "[ ] Debugging Extensions\n");
    vga_write_string(info.features.has_pse ? "[X] PSE\n" : "[ ] PSE\n");
    vga_write_string(info.features.has_tsc ? "[X] TSC\n" : "[ ] TSC\n");
    vga_write_string(info.features.has_msr ? "[X] MSR\n" : "[ ] MSR\n");
    vga_write_string(info.features.has_pae ? "[X] PAE\n" : "[ ] PAE\n");
    vga_write_string(info.features.has_mce ? "[X] MCE\n" : "[ ] MCE\n");
    vga_write_string(info.features.has_cx8 ? "[X] CMPXCHG8B\n" : "[ ] CMPXCHG8B\n");
    vga_write_string(info.features.has_apic ? "[X] APIC\n" : "[ ] APIC\n");
    vga_write_string(info.features.has_mmx ? "[X] MMX\n" : "[ ] MMX\n");
    vga_write_string(info.features.has_sse ? "[X] SSE\n" : "[ ] SSE\n");
    vga_write_string(info.features.has_sse2 ? "[X] SSE2\n" : "[ ] SSE2\n");
}