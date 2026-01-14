#include "process.h"
#include "memory.h"
#include "../drivers/vga.h"
#include <stddef.h>

// Process table
static process_t process_table[MAX_PROCESSES];
static uint32_t next_pid = 1;
static uint32_t process_count_var = 0;

// Scheduler state
process_t *current_process = NULL;
process_t *ready_queue = NULL;

// Shell return context (for returning from process execution)
cpu_context_t shell_context;
int should_return_to_shell = 0;

// External timer ticks
extern volatile uint32_t timer_ticks;

// Helper: String copy
static void string_copy(char *dest, const char *src, uint32_t max_len) {
    uint32_t i;
    for (i = 0; i < max_len - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

void process_init() {
    vga_write_string("[*] Initializing process management...\n");
    
    // Clear process table
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid = 0;
        process_table[i].state = PROCESS_TERMINATED;
    }
    
    ready_queue = NULL;
    current_process = NULL;
    process_count_var = 0;
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("[+] Process management initialized\n");
    vga_write_string("[+] Ready for multitasking (max 8 processes)\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void scheduler_init() {
    // Scheduler uses process_init()
    process_init();
}

// Find free slot in process table
static process_t* get_free_process_slot() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        // First try to find completely unused slots (pid == 0)
        if (process_table[i].pid == 0) {
            return &process_table[i];
        }
    }
    // If no unused slots, reuse terminated processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_TERMINATED) {
            // Clear the slot for reuse
            process_table[i].pid = 0;
            return &process_table[i];
        }
    }
    return NULL;
}

// Add process to ready queue
void scheduler_add_process(process_t *proc) {
    if (!proc) return;
    
    proc->next = NULL;
    
    if (!ready_queue) {
        ready_queue = proc;
    } else {
        // Add to end of queue
        process_t *current = ready_queue;
        while (current->next) {
            current = current->next;
        }
        current->next = proc;
    }
}

// Remove process from ready queue
void remove_from_ready_queue(process_t *proc) {
    if (!proc || !ready_queue) return;
    
    if (ready_queue == proc) {
        ready_queue = proc->next;
        proc->next = NULL;
        return;
    }
    
    process_t *current = ready_queue;
    while (current->next) {
        if (current->next == proc) {
            current->next = proc->next;
            proc->next = NULL;
            return;
        }
        current = current->next;
    }
}

process_t* process_create(const char *name, void (*entry_point)(), uint32_t priority) {
    process_t *proc = get_free_process_slot();
    if (!proc) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write_string("[!] Cannot create process: table full\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return NULL;
    }
    
    // Allocate kernel stack
    uint32_t *stack = (uint32_t *)kmalloc(PROCESS_STACK_SIZE);
    if (!stack) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_write_string("[!] Cannot allocate stack for process\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        return NULL;
    }
    
    // Initialize PCB
    proc->pid = next_pid++;
    string_copy(proc->name, name, 32);
    proc->state = PROCESS_READY;
    proc->stack = stack;
    proc->stack_size = PROCESS_STACK_SIZE;
    proc->priority = priority;
    proc->time_slice = DEFAULT_TIME_SLICE;
    proc->total_time = 0;
    proc->next = NULL;
    
    // Setup initial context
    // Initialize all registers to 0
    proc->context.eax = 0;
    proc->context.ebx = 0;
    proc->context.ecx = 0;
    proc->context.edx = 0;
    proc->context.esi = 0;
    proc->context.edi = 0;
    proc->context.ebp = 0;
    
    // Stack grows downward - ESP points to top of stack
    proc->context.esp = (uint32_t)stack + PROCESS_STACK_SIZE;
    
    // Set entry point
    proc->context.eip = (uint32_t)entry_point;
    
    // Set flags (interrupts enabled, reserved bit set)
    proc->context.eflags = 0x0202;
    
    // Set segment registers (all use kernel data segment)
    proc->context.cs = 0x08;   // Kernel code segment
    proc->context.ds = 0x10;   // Kernel data segment
    proc->context.es = 0x10;
    proc->context.fs = 0x10;
    proc->context.gs = 0x10;
    proc->context.ss = 0x10;
    
    proc->context.cr3 = 0;     // No paging yet
    
    process_count_var++;
    scheduler_add_process(proc);
    
    return proc;
}

process_t* scheduler_get_current() {
    return current_process;
}

// Round-robin scheduler - called by timer interrupt
void scheduler_schedule() {
    if (!ready_queue) {
        // No processes to run
        return;
    }
    
    // If no current process, pick first from queue
    if (!current_process) {
        current_process = ready_queue;
        remove_from_ready_queue(current_process);
        current_process->state = PROCESS_RUNNING;
        current_process->time_slice = DEFAULT_TIME_SLICE;
        // Don't switch yet - just mark it as current
        // On next tick, if time_slice > 0, it will just decrement
        // We can't context switch from interrupt to a fresh process easily
        return;
    }
    
    // Decrement time slice and track CPU time
    if (current_process->time_slice > 0) {
        current_process->time_slice--;
        current_process->total_time++;
    }
    
    // If time slice expired
    if (current_process->time_slice == 0 && ready_queue) {
        // Save current process
        process_t *old_process = current_process;
        
        // Get next process from queue
        current_process = ready_queue;
        remove_from_ready_queue(current_process);
        current_process->state = PROCESS_RUNNING;
        current_process->time_slice = DEFAULT_TIME_SLICE;
        
        // Put old process back in queue if still ready
        if (old_process->state == PROCESS_RUNNING) {
            old_process->state = PROCESS_READY;
            old_process->time_slice = DEFAULT_TIME_SLICE;
            scheduler_add_process(old_process);
        }
        
        // Perform context switch
        context_switch(&old_process->context, &current_process->context);
    } else if (current_process->time_slice == 0) {
        // No other process,  reset time slice
        current_process->time_slice = DEFAULT_TIME_SLICE;
    }
}

void process_yield() {
    if (current_process) {
        current_process->time_slice = 0;  // Force reschedule
    }
}

void process_exit(int status) {
    (void)status;
    
    if (!current_process) return;
    
    vga_write_string("\n[PROCESS] ");
    vga_write_string(current_process->name);
    vga_write_string(" (PID ");
    
    // Print PID
    char pid_str[12];
    int i = 0;
    uint32_t pid = current_process->pid;
    if (pid == 0) {
        pid_str[i++] = '0';
    } else {
        char temp[12];
        int j = 0;
        while (pid > 0) {
            temp[j++] = '0' + (pid % 10);
            pid /= 10;
        }
        while (j > 0) {
            pid_str[i++] = temp[--j];
        }
    }
    pid_str[i] = '\0';
    vga_write_string(pid_str);
    vga_write_string(") exited\n");
    
    // Free stack
    if (current_process->stack) {
        kfree(current_process->stack);
    }
    
    // Mark as terminated (keep PID so ps can still see it)
    current_process->state = PROCESS_TERMINATED;
    current_process->time_slice = 0;  // Clear time slice for terminated process
    // Don't clear PID here - keep it for ps to display
    // current_process->pid = 0;  // Commented out so terminated processes are visible
    process_count_var--;
    
    // Remove from queue
    remove_from_ready_queue(current_process);
    process_t *dead_process = current_process;
    current_process = NULL;
    
    // Check if there are more processes to run
    if (ready_queue) {
        // Switch to next process
        current_process = ready_queue;
        remove_from_ready_queue(current_process);
        current_process->state = PROCESS_RUNNING;
        current_process->time_slice = DEFAULT_TIME_SLICE;
        
        // Jump directly to next process (never return)
        context_switch(NULL, &current_process->context);
        
        // Should never reach here
        vga_write_string("[ERROR] Returned from context switch!\n");
        while(1);
    } else {
        // No more processes - all done!
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write_string("\n[SCHEDULER] All processes completed!\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        
        // Return to shell if we saved the context
        if (should_return_to_shell) {
            should_return_to_shell = 0;
            context_switch(NULL, &shell_context);
        }
        
        // If no shell context, halt
        while(1) {
            asm volatile("hlt");
        }
    }
}

void process_kill(uint32_t pid) {
    process_t *proc = process_get_by_pid(pid);
    if (!proc) return;
    
    if (proc->stack) {
        kfree(proc->stack);
    }
    
    proc->state = PROCESS_TERMINATED;
    proc->pid = 0;
    process_count_var--;
    
    remove_from_ready_queue(proc);
    
    if (current_process == proc) {
        current_process = NULL;
        scheduler_schedule();
    }
}

// Cleanup all remaining processes (called when returning to shell)
void process_cleanup_all() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid != 0 && process_table[i].state != PROCESS_TERMINATED) {
            process_table[i].state = PROCESS_TERMINATED;
            process_table[i].time_slice = 0;
        }
    }
}

process_t* process_get_by_pid(uint32_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid) {
            return &process_table[i];
        }
    }
    return NULL;
}

uint32_t process_count() {
    return process_count_var;
}

void process_list_all() {
    vga_write_string("\n=== Process List ===\n");
    vga_write_string("PID  State      Name                 Time    Slice\n");
    vga_write_string("---  ---------  -------------------  ------  -----\n");
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid != 0) {
            process_t *p = &process_table[i];
            
            // PID
            char str[12];
            int idx = 0;
            uint32_t num = p->pid;
            if (num == 0) {
                str[idx++] = '0';
            } else {
                char temp[12];
                int j = 0;
                while (num > 0) {
                    temp[j++] = '0' + (num % 10);
                    num /= 10;
                }
                while (j > 0) {
                    str[idx++] = temp[--j];
                }
            }
            str[idx] = '\0';
            vga_write_string(str);
            vga_write_string("    ");
            
            // State
            const char *state_str = "UNKNOWN";
            if (p->state == PROCESS_READY) state_str = "READY    ";
            else if (p->state == PROCESS_RUNNING) state_str = "RUNNING  ";
            else if (p->state == PROCESS_BLOCKED) state_str = "BLOCKED  ";
            else if (p->state == PROCESS_TERMINATED) state_str = "TERMINATED";
            vga_write_string(state_str);
            vga_write_string("  ");
            
            // Name
            vga_write_string(p->name);
            
            // Padding
            int name_len = 0;
            while (p->name[name_len] && name_len < 32) name_len++;
            for (int pad = name_len; pad < 20; pad++) {
                vga_write_string(" ");
            }
            
            // Total time
            idx = 0;
            num = p->total_time;
            if (num == 0) {
                str[idx++] = '0';
            } else {
                char temp[12];
                int j = 0;
                while (num > 0) {
                    temp[j++] = '0' + (num % 10);
                    num /= 10;
                }
                while (j > 0) {
                    str[idx++] = temp[--j];
                }
            }
            str[idx] = '\0';
            vga_write_string(str);
            vga_write_string("      ");
            
            // Time slice
            idx = 0;
            num = p->time_slice;
            if (num == 0) {
                str[idx++] = '0';
            } else {
                char temp[12];
                int j = 0;
                while (num > 0) {
                    temp[j++] = '0' + (num % 10);
                    num /= 10;
                }
                while (j > 0) {
                    str[idx++] = temp[--j];
                }
            }
            str[idx] = '\0';
            vga_write_string(str);
            
            vga_write_string("\n");
        }
    }
    
    // Count active processes
    uint32_t active = 0;
    uint32_t total = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid != 0) {
            total++;
            if (process_table[i].state != PROCESS_TERMINATED) {
                active++;
            }
        }
    }
    
    vga_write_string("\nActive processes: ");
    char count_str[12];
    int idx = 0;
    if (active == 0) {
        count_str[idx++] = '0';
    } else {
        char temp[12];
        int j = 0;
        uint32_t count = active;
        while (count > 0) {
            temp[j++] = '0' + (count % 10);
            count /= 10;
        }
        while (j > 0) {
            count_str[idx++] = temp[--j];
        }
    }
    count_str[idx] = '\0';
    vga_write_string(count_str);
    
    vga_write_string(" / Total in table: ");
    idx = 0;
    if (total == 0) {
        count_str[idx++] = '0';
    } else {
        char temp[12];
        int j = 0;
        while (total > 0) {
            temp[j++] = '0' + (total % 10);
            total /= 10;
        }
        while (j > 0) {
            count_str[idx++] = temp[--j];
        }
    }
    count_str[idx] = '\0';
    vga_write_string(count_str);
    vga_write_string("\n\n");
}

// Fork implementation (simplified - no copy-on-write)
int process_fork() {
    if (!current_process) {
        return -1;  // No current process
    }
    
    // Create child process
    process_t *child = get_free_process_slot();
    if (!child) {
        return -1;  // No free slots
    }
    
    // Allocate stack for child
    uint32_t *child_stack = (uint32_t *)kmalloc(PROCESS_STACK_SIZE);
    if (!child_stack) {
        return -1;  // Out of memory
    }
    
    // Copy parent's name with " (child)" suffix
    string_copy(child->name, current_process->name, 24);
    char *suffix = " (child)";
    int name_len = 0;
    while (child->name[name_len] && name_len < 32) name_len++;
    for (int i = 0; suffix[i] && (name_len + i) < 31; i++) {
        child->name[name_len + i] = suffix[i];
    }
    
    // Copy parent state
    child->pid = next_pid++;
    child->state = PROCESS_READY;
    child->stack = child_stack;
    child->stack_size = PROCESS_STACK_SIZE;
    child->priority = current_process->priority;
    child->time_slice = DEFAULT_TIME_SLICE;
    child->total_time = 0;
    child->next = NULL;
    
    // Copy context
    child->context = current_process->context;
    
    // Copy stack contents
    for (uint32_t i = 0; i < PROCESS_STACK_SIZE / 4; i++) {
        child_stack[i] = current_process->stack[i];
    }
    
    // Adjust child's stack pointer
    uint32_t stack_offset = current_process->context.esp - (uint32_t)current_process->stack;
    child->context.esp = (uint32_t)child_stack + stack_offset;
    
    process_count_var++;
    scheduler_add_process(child);
    
    // Return child PID to parent, 0 to child (will be set during context switch)
    return child->pid;
}
