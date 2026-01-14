#include "shell.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "memory.h"
#include "kim.h"
#include "rootkit.h"
#include "process.h"
#include "vfs.h"
#include "sysinfo.h"
#include "cpu_info.h"
#include "pci.h"
#include <stdint.h>

#define MAX_INPUT 256

static char command_buffer[MAX_INPUT];
static int buffer_pos = 0;

// String comparison
static int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// String copy
static void strcpy(char *dest, const char *src)
{
    while ((*dest++ = *src++))
        ;
}

// String length
static int strlen(const char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return len;
}

// String compare with length limit
static int strncmp(const char *s1, const char *s2, int n)
{
    for (int i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0')
            return s1[i] - s2[i];
    }
    return 0;
}

void shell_init()
{
    vga_write_string("\n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("   MiniKernel-Sec Shell v0.1\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_write_string("Type 'help' for available commands\n\n");
}

static void print_prompt()
{
    // Get current theme colors
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    // Use green for prompt, but keep current background
    vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
    vga_write_string("kernel> ");
    vga_set_color(fg, bg);
}

static void execute_command(const char *cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        vga_write_string("\nAvailable commands:\n");
        vga_write_string("  help      - Show this help message\n");
        vga_write_string("  clear     - Clear the screen\n");
        vga_write_string("  sysinfo   - Display system and CPU information\n");
        vga_write_string("  pci       - Scan and display PCI devices\n");
        vga_write_string("  meminfo   - Display memory information\n");
        vga_write_string("  about     - Show kernel information\n");
        vga_write_string("  test      - Test memory allocator\n");
        vga_write_string("  syscall   - Test system call interface\n");
        vga_write_string("  ps        - List all processes\n");
        vga_write_string("  demo      - Run multitasking demo\n");
        vga_write_string("  ls        - List files in RAM disk\n");
        vga_write_string("  cat       - Read and display a file\n");
        vga_write_string("  write     - Write to a file\n");
        vga_write_string("  touch     - Create a new file\n");
        vga_write_string("  rm        - Delete a file\n");
        vga_write_string("  theme     - Set theme (dark/light)\n");
        vga_write_string("  idtcheck  - Check IDT integrity\n");
        vga_write_string("  idtinfo   - Display IDT information\n");
        vga_write_string("  funccheck - Check function integrity (CRC32)\n");
        vga_write_string("  attack    - Simulate rootkit IDT hook attack\n");
        vga_write_string("  unhook    - Remove rootkit IDT hook\n");
        vga_write_string("  patch     - Simulate inline hook (function patching)\n");
        vga_write_string("  unpatch   - Remove inline hook\n");
        vga_write_string("\n");
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        vga_clear();
    }
    else if (strcmp(cmd, "sysinfo") == 0)
    {
        vga_write_string("\n");
        sysinfo_display();
    }
    else if (strcmp(cmd, "pci") == 0)
    {
        pci_print_devices();
        vga_write_string("\n");
    }
    else if (strcmp(cmd, "meminfo") == 0)
    {
        memory_info();
    }
    else if (strcmp(cmd, "about") == 0)
    {
        vga_write_string("\nMiniKernel-Sec v0.1\n");
        vga_write_string("A minimalist kernel with security features\n");
        vga_write_string("Built for educational purposes\n\n");
    }
    else if (strcmp(cmd, "test") == 0)
    {
        vga_write_string("\n[*] Testing memory allocator...\n");

        void *ptr1 = kmalloc(100);
        vga_write_string("[+] Allocated 100 bytes at: 0x");
        vga_write_string("...\n");

        void *ptr2 = kmalloc(200);
        vga_write_string("[+] Allocated 200 bytes at: 0x");
        vga_write_string("...\n");

        kfree(ptr1);
        vga_write_string("[+] Freed first allocation\n");

        void *ptr3 = kmalloc(50);
        vga_write_string("[+] Allocated 50 bytes at: 0x");
        vga_write_string("...\n");

        kfree(ptr2);
        kfree(ptr3);
        vga_write_string("[+] All memory freed\n");
        vga_write_string("[+] Memory allocator test complete!\n\n");
    }
    else if (strcmp(cmd, "syscall") == 0)
    {
        vga_write_string("\n[*] Testing system call interface...\n\n");
        
        // Test SYS_GETPID
        int pid;
        __asm__ volatile(
            "mov $3, %%eax\n"      // SYS_GETPID
            "int $0x80\n"
            "mov %%eax, %0"
            : "=r"(pid)
            :
            : "eax"
        );
        vga_write_string("[SYSCALL TEST] getpid() returned: ");
        char pid_str[12];
        int i = 0;
        if (pid == 0) {
            pid_str[i++] = '0';
        } else {
            char temp[12];
            int j = 0;
            int num = pid;
            while (num > 0) {
                temp[j++] = '0' + (num % 10);
                num /= 10;
            }
            while (j > 0) {
                pid_str[i++] = temp[--j];
            }
        }
        pid_str[i] = '\0';
        vga_write_string(pid_str);
        vga_write_string("\n");
        
        // Test SYS_UPTIME
        int uptime;
        __asm__ volatile(
            "mov $4, %%eax\n"      // SYS_UPTIME
            "int $0x80\n"
            "mov %%eax, %0"
            : "=r"(uptime)
            :
            : "eax"
        );
        vga_write_string("[SYSCALL TEST] uptime() returned: ");
        i = 0;
        if (uptime == 0) {
            pid_str[i++] = '0';
        } else {
            char temp[12];
            int j = 0;
            int num = uptime;
            while (num > 0) {
                temp[j++] = '0' + (num % 10);
                num /= 10;
            }
            while (j > 0) {
                pid_str[i++] = temp[--j];
            }
        }
        pid_str[i] = '\0';
        vga_write_string(pid_str);
        vga_write_string(" ticks\n");
        
        // Test SYS_WRITE
        const char *test_msg = "Hello from syscall!\n";
        int written;
        __asm__ volatile(
            "mov $1, %%eax\n"      // SYS_WRITE
            "mov $1, %%ebx\n"      // fd = stdout
            "mov %1, %%ecx\n"      // buffer
            "mov $20, %%edx\n"     // count
            "int $0x80\n"
            "mov %%eax, %0"
            : "=r"(written)
            : "r"(test_msg)
            : "eax", "ebx", "ecx", "edx"
        );
        vga_write_string("[SYSCALL TEST] write(1, \"Hello from syscall!\\n\", 20) returned: ");
        i = 0;
        if (written == 0) {
            pid_str[i++] = '0';
        } else {
            char temp[12];
            int j = 0;
            int num = written;
            while (num > 0) {
                temp[j++] = '0' + (num % 10);
                num /= 10;
            }
            while (j > 0) {
                pid_str[i++] = temp[--j];
            }
        }
        pid_str[i] = '\0';
        vga_write_string(pid_str);
        vga_write_string("\n");
        
        // Test SYS_EXIT (just show it, don't actually call or kernel will halt)
        vga_write_string("[SYSCALL TEST] exit(0) - skipped (would terminate kernel)\n");
        
        vga_write_string("\n");
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write_string("[+] System call tests complete!\n\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
    else if (strcmp(cmd, "idtcheck") == 0)
    {
        kim_check_idt();
    }
    else if (strcmp(cmd, "idtinfo") == 0)
    {
        kim_display_idt_info();
    }
    else if (strcmp(cmd, "funccheck") == 0)
    {
        kim_check_functions();
    }
    else if (strcmp(cmd, "attack") == 0)
    {
        rootkit_hook_keyboard();
    }
    else if (strcmp(cmd, "unhook") == 0)
    {
        rootkit_unhook();
    }
    else if (strcmp(cmd, "patch") == 0)
    {
        rootkit_patch_function();
    }
    else if (strcmp(cmd, "unpatch") == 0)
    {
        rootkit_unpatch_function();
    }
    else if (strcmp(cmd, "ls") == 0)
    {
        vfs_list_files();
    }
    else if (strncmp(cmd, "cat ", 4) == 0)
    {
        const char *filename = cmd + 4;
        char buffer[256];
        int bytes = vfs_read_file(filename, buffer, 255);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            vga_write_string("\nFile: ");
            vga_write_string(filename);
            vga_write_string("\n");
            vga_write_string(buffer);
            vga_write_string("\n\n");
        } else {
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_write_string("\n[!] File not found: ");
            vga_write_string(filename);
            vga_write_string("\n\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
    else if (strncmp(cmd, "write ", 6) == 0)
    {
        // Parse: echo filename data (data can have spaces)
        const char *rest = cmd + 5;
        
        // Skip leading spaces
        while (*rest == ' ') rest++;
        
        // Extract filename (until first space)
        char filename[32];
        int i = 0;
        while (rest[i] != ' ' && rest[i] != '\0' && i < 31) {
            filename[i] = rest[i];
            i++;
        }
        filename[i] = '\0';
        
        if (rest[i] == ' ') {
            // Skip spaces between filename and data
            const char *data = rest + i;
            while (*data == ' ') data++;
            
            if (*data != '\0') {
                if (vfs_write_file(filename, data) >= 0) {
                    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
                    vga_write_string("\n[+] Written to ");
                    vga_write_string(filename);
                    vga_write_string("\n\n");
                    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
                } else {
                    vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
                    vga_write_string("\n[!] Failed to write to file\n\n");
                    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
                }
            } else {
                vga_write_string("\nUsage: echo filename data\n\n");
            }
        } else {
            vga_write_string("\nUsage: echo filename data\n\n");
        }
    }
    else if (strncmp(cmd, "touch ", 6) == 0)
    {
        const char *filename = cmd + 6;
        if (vfs_create_file(filename) == 0) {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_write_string("\n[+] Created file: ");
            vga_write_string(filename);
            vga_write_string("\n\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        } else {
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_write_string("\n[!] Failed to create file (may already exist or disk full)\n\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
    else if (strncmp(cmd, "rm ", 3) == 0)
    {
        const char *filename = cmd + 3;
        if (vfs_delete_file(filename) == 0) {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_write_string("\n[+] Deleted file: ");
            vga_write_string(filename);
            vga_write_string("\n\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        } else {
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_write_string("\n[!] File not found: ");
            vga_write_string(filename);
            vga_write_string("\n\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
    else if (strncmp(cmd, "theme ", 6) == 0)
    {
        const char *theme = cmd + 6;
        if (strcmp(theme, "dark") == 0) {
            vga_apply_dark_theme();
            vga_write_string("\n[+] Dark theme applied\n\n");
            // Update config file
            vfs_write_file("os.conf", "theme=dark;user=admin;");
        } else if (strcmp(theme, "light") == 0) {
            vga_apply_light_theme();
            vga_write_string("\n[+] Light theme applied\n\n");
            // Update config file
            vfs_write_file("os.conf", "theme=light;user=admin;");
        } else {
            vga_write_string("\nUsage: theme dark|light\n\n");
        }
    }
    else if (strcmp(cmd, "ps") == 0)
    {
        process_list_all();
    }
    else if (strcmp(cmd, "demo") == 0)
    {
        vga_write_string("\n[*] Starting multitasking demo...\n");
        vga_write_string("[*] Creating 3 processes that will run concurrently\n\n");
        
        // Create demo processes
        extern void demo_process_a();
        extern void demo_process_b();
        extern void demo_process_c();
        
        process_t *proc_a = process_create("Process-A", demo_process_a, 0);
        process_t *proc_b = process_create("Process-B", demo_process_b, 0);
        process_t *proc_c = process_create("Process-C", demo_process_c, 0);
        
        if (proc_a && proc_b && proc_c) {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_write_string("[+] Created 3 processes\n");
            vga_write_string("[+] Starting process execution...\n\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            
            // Start running the processes (this never returns)
            run_processes();
        } else {
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_write_string("[!] Failed to create all processes\n\n");
            vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
    else if (strlen(cmd) > 0)
    {
        vga_write_string("Unknown command: ");
        vga_write_string(cmd);
        vga_write_string("\nType 'help' for available commands\n");
    }
}

void shell_run()
{
    buffer_pos = 0;

    while (1)
    {
        print_prompt();

        // Read command
        while (1)
        {
            char c = keyboard_getchar();

            if (c == '\n')
            {
                vga_putchar('\n');
                command_buffer[buffer_pos] = '\0';
                execute_command(command_buffer);
                buffer_pos = 0;
                break;
            }
            else if (c == '\b')
            {
                if (buffer_pos > 0)
                {
                    buffer_pos--;
                    vga_putchar('\b');
                }
            }
            else if (buffer_pos < MAX_INPUT - 1)
            {
                command_buffer[buffer_pos++] = c;
                vga_putchar(c);
            }
        }
    }
}