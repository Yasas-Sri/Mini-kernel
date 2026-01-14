#include "syscall.h"
#include "idt.h"
#include "process.h"
#include "../drivers/vga.h"
#include "../drivers/pit.h"
#include <stddef.h>

// Syscall table - array of function pointers
static syscall_handler_t syscall_table[SYSCALL_MAX];

// Current process ID (for demo purposes, static for now)
static uint32_t current_pid = 1;

// Assembly syscall entry point (defined in syscall_entry.asm)
extern void syscall_entry();

void syscall_init()
{
    vga_write_string("[*] Installing system call interface...\n");
    
    // Register syscall handlers
    syscall_table[SYS_EXIT] = sys_exit;
    syscall_table[SYS_WRITE] = sys_write;
    syscall_table[SYS_READ] = sys_read;
    syscall_table[SYS_GETPID] = sys_getpid;
    syscall_table[SYS_UPTIME] = sys_uptime;
    syscall_table[SYS_FORK] = sys_fork;
    
    // Install INT 0x80 handler in IDT
    idt_set_gate(0x80, (uint32_t)syscall_entry, 0x08, 0xEE);
    // 0xEE = 11101110b
    // Present=1, DPL=3 (Ring 3 can call), Type=1110 (32-bit interrupt gate)
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] System calls installed (int 0x80)\n");
    vga_write_string("[+] Registered 6 syscalls: exit, write, read, getpid, uptime, fork\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

// Syscall dispatcher - routes to appropriate handler
void syscall_handler(struct syscall_registers *regs)
{
    // Validate syscall number
    if (regs->eax >= SYSCALL_MAX)
    {
        // Invalid syscall number, return -1
        regs->eax = (uint32_t)-1;
        return;
    }
    
    // Get handler from table
    syscall_handler_t handler = syscall_table[regs->eax];
    
    if (!handler)
    {
        // No handler registered
        regs->eax = (uint32_t)-1;
        return;
    }
    
    // Call the handler with arguments from registers
    int result = handler(regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
    
    // Return value goes in EAX
    regs->eax = result;
}

// SYS_EXIT - Terminate process
int sys_exit(uint32_t status, uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4)
{
    (void)unused1; (void)unused2; (void)unused3; (void)unused4;
    
    vga_write_string("\n[SYSCALL] exit(");
    
    // Print status code
    char status_str[12];
    int i = 0;
    uint32_t num = status;
    
    if (num == 0) {
        status_str[i++] = '0';
    } else {
        char temp[12];
        int j = 0;
        while (num > 0) {
            temp[j++] = '0' + (num % 10);
            num /= 10;
        }
        while (j > 0) {
            status_str[i++] = temp[--j];
        }
    }
    status_str[i] = '\0';
    
    vga_write_string(status_str);
    vga_write_string(") called\n");
    vga_write_string("[SYSCALL] Process terminated\n\n");
    
    return 0;
}

// SYS_WRITE - Write to file descriptor (fd 1 = stdout)
int sys_write(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t unused1, uint32_t unused2)
{
    (void)unused1; (void)unused2;
    
    // Only support stdout (fd=1) for now
    if (fd != 1)
        return -1;
    
    // Validate buffer pointer (should be in user space, but we'll skip for now)
    if (buffer == 0 || count == 0)
        return -1;
    
    // Write characters to VGA
    char *str = (char *)buffer;
    uint32_t written = 0;
    
    for (uint32_t i = 0; i < count; i++)
    {
        if (str[i] == '\0')
            break;
        
        char c[2] = {str[i], '\0'};
        vga_write_string(c);
        written++;
    }
    
    return written;
}

// SYS_READ - Read from file descriptor (not implemented yet)
int sys_read(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t unused1, uint32_t unused2)
{
    (void)fd; (void)buffer; (void)count;
    (void)unused1; (void)unused2;
    
    // TODO: Implement keyboard reading via syscall
    return -1;
}

// SYS_GETPID - Get process ID
int sys_getpid(uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4, uint32_t unused5)
{
    (void)unused1; (void)unused2; (void)unused3; (void)unused4; (void)unused5;
    return current_pid;
}

// SYS_UPTIME - Get system uptime in ticks
int sys_uptime(uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4, uint32_t unused5)
{
    (void)unused1; (void)unused2; (void)unused3; (void)unused4; (void)unused5;
    
    extern uint32_t timer_ticks;
    return timer_ticks;
}

// SYS_FORK - Create new process
int sys_fork(uint32_t unused1, uint32_t unused2, uint32_t unused3, uint32_t unused4, uint32_t unused5)
{
    (void)unused1; (void)unused2; (void)unused3; (void)unused4; (void)unused5;
    return process_fork();
}
