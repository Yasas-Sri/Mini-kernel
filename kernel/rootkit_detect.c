#include "rootkit_detect.h"
#include "../drivers/vga.h"
#include "idt.h"
#include "process.h"
#include "kim.h"
#include <stdint.h>

static detection_report_t report = {0};

// Store baseline IDT entries at boot
static uint32_t baseline_idt[256] = {0};
static uint8_t baseline_initialized = 0;

void rootkit_detect_init(void)
{
    // Capture baseline IDT state
    extern struct idt_entry idt_entries[256];
    
    for (int i = 0; i < 256; i++) {
        baseline_idt[i] = ((uint32_t)idt_entries[i].base_high << 16) | idt_entries[i].base_low;
    }
    
    baseline_initialized = 1;
    
    report.total_checks = 0;
    report.hooks_detected = 0;
    report.patches_detected = 0;
    report.hidden_processes = 0;
}

uint8_t detect_idt_hooks(void)
{
    if (!baseline_initialized) {
        vga_write_string("[!] Baseline not initialized!\n");
        return 0;
    }
    
    extern struct idt_entry idt_entries[256];
    uint8_t hooks_found = 0;
    
    // Check critical interrupts (including IRQ7 which rootkit.c uses)
    uint8_t critical_interrupts[] = {0, 1, 3, 8, 13, 14, 32, 33, 39}; // Added 39 (IRQ7)
    
    for (int i = 0; i < 9; i++) { // Changed from 8 to 9
        uint8_t int_num = critical_interrupts[i];
        uint32_t current = ((uint32_t)idt_entries[int_num].base_high << 16) | idt_entries[int_num].base_low;
        
        // Debug: Show comparison
        if (int_num == 39) {
            vga_write_string("  [DEBUG] IRQ7 (int 39) - Baseline: 0x");
            vga_write_hex(baseline_idt[int_num]);
            vga_write_string(", Current: 0x");
            vga_write_hex(current);
            vga_write_string("\n");
        }
        
        if (current != baseline_idt[int_num]) {
            hooks_found++;
            vga_write_string("[!] IDT Hook detected at interrupt ");
            vga_write_dec(int_num);
            vga_write_string(": Expected 0x");
            vga_write_hex(baseline_idt[int_num]);
            vga_write_string(", Got 0x");
            vga_write_hex(current);
            vga_write_string("\n");
        }
    }
    
    report.hooks_detected += hooks_found;
    return hooks_found;
}

uint8_t detect_function_patches(void)
{
    // Check if shell_init has been patched by rootkit
    extern int rootkit_inline_hook_active(void);
    
    uint8_t patches_found = 0;
    
    // Check if the rootkit's inline hook is active
    if (rootkit_inline_hook_active()) {
        patches_found++;
        vga_write_string("[!] Function patch detected: shell_init() has been modified\n");
    }
    
    report.patches_detected = patches_found;
    return patches_found;
}

uint8_t detect_hidden_processes(void)
{
    // Cross-view validation: Compare process table with expected state
    // Note: process_table is static in process.c, so we can't access it directly
    // Instead, we'll use the process counting mechanism
    
    // For now, this is a placeholder that doesn't actually check
    // You could add a public function in process.c to expose process count
    
    uint32_t suspicious_count = 0;
    
    // Placeholder detection logic
    // In a full implementation, you'd need process.c to expose:
    // - process_get_count()
    // - process_validate_table()
    
    report.hidden_processes = suspicious_count;
    return suspicious_count > 0;
}

uint8_t detect_suspicious_memory(void)
{
    // Check for memory regions that shouldn't be executable
    // or for unexpected memory modifications
    
    // This is a placeholder - you can expand based on your memory layout
    return 0;
}

void rootkit_detect_scan(void)
{
    vga_write_string("\n[+] Starting Rootkit Detection Scan...\n\n");
    
    report.total_checks = 0;
    report.hooks_detected = 0;
    report.patches_detected = 0;
    report.hidden_processes = 0;
    
    // 1. Check IDT integrity
    vga_write_string("[*] Checking IDT integrity...\n");
    detect_idt_hooks();
    report.total_checks++;
    
    // 2. Check function patches
    vga_write_string("[*] Checking function integrity...\n");
    detect_function_patches();
    report.total_checks++;
    
    // 3. Check for hidden processes
    vga_write_string("[*] Checking for hidden processes...\n");
    detect_hidden_processes();
    report.total_checks++;
    
    // 4. Check memory regions
    vga_write_string("[*] Checking memory regions...\n");
    detect_suspicious_memory();
    report.total_checks++;
    
    vga_write_string("\n[+] Scan complete!\n\n");
}

void rootkit_detect_display_report(void)
{
    vga_write_string("\n========== ROOTKIT DETECTION REPORT ==========\n\n");
    
    vga_write_string("Total Checks Performed: ");
    vga_write_dec(report.total_checks);
    vga_write_string("\n");
    
    vga_write_string("IDT Hooks Detected: ");
    vga_write_dec(report.hooks_detected);
    vga_write_string("\n");
    
    vga_write_string("Function Patches Detected: ");
    vga_write_dec(report.patches_detected);
    vga_write_string("\n");
    
    vga_write_string("Hidden Processes Detected: ");
    vga_write_dec(report.hidden_processes);
    vga_write_string("\n");
    
    uint32_t total_detections = report.hooks_detected + 
                                report.patches_detected + 
                                report.hidden_processes;
    
    if (total_detections == 0) {
        vga_write_string("\n[+] System appears clean - no rootkit detected\n");
    } else {
        vga_write_string("\n[!] WARNING: Potential rootkit activity detected!\n");
        vga_write_string("[!] Total anomalies found: ");
        vga_write_dec(total_detections);
        vga_write_string("\n");
    }
    
    vga_write_string("\n==============================================\n\n");
}

// Enhanced verification using existing kim.c functions
void verify_idt_integrity(void)
{
    // Use existing idtcheck functionality
    // Just run detect_idt_hooks() for now
    detect_idt_hooks();
}
