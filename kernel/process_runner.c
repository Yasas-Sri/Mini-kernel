#include <stddef.h>
#include "process.h"
#include "../drivers/vga.h"

// External references to scheduler internals
extern process_t *current_process;
extern process_t *ready_queue;
extern void remove_from_ready_queue(process_t *proc);
extern cpu_context_t shell_context;
extern int should_return_to_shell;

// Simple process runner - starts executing processes
// Saves shell context and jumps to first process
void run_processes() {
    if (!ready_queue) {
        vga_write_string("[SCHEDULER] No processes to run!\n");
        return;
    }
    
    // Pick first process
    current_process = ready_queue;
    remove_from_ready_queue(current_process);
    current_process->state = PROCESS_RUNNING;
    current_process->time_slice = DEFAULT_TIME_SLICE;
    
    vga_write_string("[SCHEDULER] Starting first process: ");
    vga_write_string(current_process->name);
    vga_write_string("\n");
    
    // Enable returning to shell when all processes complete
    should_return_to_shell = 1;
    
    // Save current context (shell) and jump to first process
    // When all processes exit, we'll return here
    context_switch(&shell_context, &current_process->context);
    
    // We return here when all processes are done!
    vga_write_string("\n[SCHEDULER] Returned to shell\n");
}
