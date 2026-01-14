#ifndef ROOTKIT_DETECT_H
#define ROOTKIT_DETECT_H

#include <stdint.h>

// IDT hook detection
typedef struct {
    uint32_t entry_number;
    uint32_t expected_offset;
    uint32_t actual_offset;
    uint8_t is_hooked;
} idt_hook_info_t;

// Function integrity check
typedef struct {
    const char *function_name;
    uint32_t expected_crc;
    uint32_t actual_crc;
    uint8_t is_patched;
} function_integrity_t;

// Detection results
typedef struct {
    uint32_t total_checks;
    uint32_t hooks_detected;
    uint32_t patches_detected;
    uint32_t hidden_processes;
} detection_report_t;

// Main detection functions
void rootkit_detect_init(void);
void rootkit_detect_scan(void);
void rootkit_detect_display_report(void);

// Specific detection methods
uint8_t detect_idt_hooks(void);
uint8_t detect_function_patches(void);
uint8_t detect_hidden_processes(void);
uint8_t detect_suspicious_memory(void);

// Helper functions
void calculate_function_crc(void *func_start, uint32_t size, uint32_t *crc);
void verify_idt_integrity(void);

#endif
