#ifndef KIM_H
#define KIM_H

#include <stdint.h>
#include <stddef.h>


#define KERNEL_START 0x00010000
#define KERNEL_END   0x00100000


#define FUNC_CHECK_SIZE 32 
#define MAX_WATCHED_FUNCTIONS 5


#define KIM_OK 0
#define KIM_IDT_COMPROMISED 1
#define KIM_FUNCTION_COMPROMISED 2


typedef struct {
    const char *name;
    uint32_t address;
    uint32_t checksum;
    int is_active;
} function_baseline_t;


void kim_init();
int kim_check_idt();
void kim_display_idt_info();


void kim_add_function_watch(const char *name, uint32_t address);
int kim_check_functions();
uint32_t kim_crc32(const uint8_t *data, size_t length);

#endif
