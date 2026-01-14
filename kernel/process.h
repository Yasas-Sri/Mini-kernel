#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

// Process states
typedef enum {
    PROCESS_READY,      // Ready to run
    PROCESS_RUNNING,    // Currently executing
    PROCESS_BLOCKED,    // Waiting for I/O
    PROCESS_TERMINATED  // Finished execution
} process_state_t;

// CPU context saved during context switch
typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip;       // Instruction pointer
    uint32_t eflags;    // CPU flags
    uint32_t cr3;       // Page directory (for future paging)
    uint16_t cs, ds, es, fs, gs, ss;
} cpu_context_t;

// Process Control Block (PCB)
typedef struct process {
    uint32_t pid;               // Process ID
    char name[32];              // Process name
    process_state_t state;      // Current state
    cpu_context_t context;      // Saved CPU context
    uint32_t *stack;            // Kernel stack pointer
    uint32_t stack_size;        // Stack size in bytes
    uint32_t priority;          // Priority (0 = highest)
    uint32_t time_slice;        // Remaining time slice (ticks)
    uint32_t total_time;        // Total CPU time used
    struct process *next;       // Next process in queue
} process_t;

// Maximum number of processes
#define MAX_PROCESSES 8
#define DEFAULT_TIME_SLICE 10  // 10 timer ticks (100ms at 100Hz)
#define PROCESS_STACK_SIZE 4096 // 4KB stack per process

// Process management functions
void process_init();
process_t* process_create(const char *name, void (*entry_point)(), uint32_t priority);
void process_exit(int status);
int process_fork();
void process_yield();
void process_kill(uint32_t pid);

// Scheduler functions
void scheduler_init();
void scheduler_schedule();  // Called by timer interrupt
process_t* scheduler_get_current();
void scheduler_add_process(process_t *proc);
void run_processes();  // Start executing processes (never returns)

// Context switching
void context_switch(cpu_context_t *old_context, cpu_context_t *new_context);

// Process list management
process_t* process_get_by_pid(uint32_t pid);
void process_list_all();
uint32_t process_count();

#endif
