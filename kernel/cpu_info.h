#ifndef CPU_INFO_H
#define CPU_INFO_H

#include <stdint.h>
#include <stddef.h>

// CPU Vendor Strings
#define CPU_VENDOR_INTEL "GenuineIntel"
#define CPU_VENDOR_AMD "AuthenticAMD"
#define CPU_VENDOR_UNKNOWN "Unknown"

// CPU Feature Flags (from CPUID)
typedef struct
{
    uint32_t has_fpu : 1;
    uint32_t has_vme : 1;
    uint32_t has_de : 1;
    uint32_t has_pse : 1;
    uint32_t has_tsc : 1;
    uint32_t has_msr : 1;
    uint32_t has_pae : 1;
    uint32_t has_mce : 1;
    uint32_t has_cx8 : 1;
    uint32_t has_apic : 1;
    uint32_t has_mmx : 1;
    uint32_t has_sse : 1;
    uint32_t has_sse2 : 1;
    uint32_t reserved : 19;
} cpu_features_t;

// CPU Information Structure
typedef struct
{
    char vendor[13];         // CPU vendor string (12 chars + null)
    char brand[48];          // CPU brand string
    cpu_features_t features; // CPU feature flags
    uint32_t family;         // CPU family
    uint32_t model;          // CPU model
    uint32_t stepping;       // CPU stepping
    uint32_t max_cpuid;      // Maximum CPUID function supported
} cpu_info_t;

// Function Prototypes
void cpu_detect(cpu_info_t *info);
void cpu_print_info(void);
const char *cpu_get_vendor(void);
void cpu_print_features(void);

#endif