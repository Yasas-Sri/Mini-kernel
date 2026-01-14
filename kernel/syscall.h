#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// System call numbers
#define SYS_EXIT    0
#define SYS_WRITE   1
#define SYS_READ    2
#define SYS_GETPID  3
#define SYS_UPTIME  4
#define SYS_FORK    5

// Maximum number of syscalls
#define SYSCALL_MAX 6

// Syscall handler function pointer type
typedef int (*syscall_handler_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

// Registers passed from user mode
struct syscall_registers {
    uint32_t eax;  // Syscall number
    uint32_t ebx;  // Argument 1
    uint32_t ecx;  // Argument 2
    uint32_t edx;  // Argument 3
    uint32_t esi;  // Argument 4
    uint32_t edi;  // Argument 5
};

// Initialize syscall system
void syscall_init();

// System call implementations
int sys_exit(uint32_t status, uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4);
int sys_write(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t unused1, uint32_t unused2);
int sys_read(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t unused1, uint32_t unused2);
int sys_getpid(uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4, uint32_t unused5);
int sys_uptime(uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4, uint32_t unused5);
int sys_fork(uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4, uint32_t unused5);

// Syscall dispatcher (called from interrupt handler)
void syscall_handler(struct syscall_registers *regs);

#endif
