# MiniKernel-Sec v0.2

A minimalist x86 kernel with security features and system call interface for educational purposes. This project implements kernel-level integrity monitoring to detect rootkit attacks and provides a POSIX-like syscall interface.

## Features

### Core Kernel (Phase 1)
- **Bootloader**: Custom NASM bootloader with BIOS disk access
- **Protected Mode**: 32-bit x86 protected mode with GDT (Ring 0 + Ring 3 segments)
- **Interrupt Handling**: Full IDT with 32 CPU exceptions + 16 hardware IRQs
- **Drivers**:
  - VGA text mode (80x25)
  - Interrupt-driven keyboard with circular buffer
  - Programmable Interval Timer (PIT) at 100Hz
- **Memory Management**: Heap allocator with kmalloc/kfree
- **Interactive Shell**: Command-line interface with 13 commands

### Security Features (Phase 2)
- **Kernel Integrity Module (KIM)**:
  - IDT integrity checker (detects interrupt hook attacks)
  - Function integrity checker (detects inline hooks via CRC32)
  - Baseline snapshot system for comparison-based detection
  
- **Mock Rootkit** (for demonstration):
  - IDT hook simulation (modifies interrupt descriptors)
  - Inline hook simulation (patches function code)
  - Safe non-executable attacks for testing

### System Call Interface (Phase 3) ⭐ NEW
- **INT 0x80 Handler**: POSIX-like syscall mechanism
- **5 System Calls**: exit, write, read, getpid, uptime
- **User/Kernel Separation**: Ring 3 user mode support
- **Syscall Table**: Extensible dispatcher architecture

## Building

### Prerequisites
- GCC multilib support (`gcc-multilib` on Arch Linux)
- NASM assembler
- GNU Make
- QEMU (for testing)

### Compilation
```bash
make
```

This builds `MiniKernelSec.bin` (1.44MB floppy disk image) and launches QEMU.

### Manual Testing
```bash
qemu-system-i386 -drive file=MiniKernelSec.bin,format=raw,if=floppy -serial stdio
```

## Usage

### Available Commands
- `help` - Show command list
- `clear` - Clear screen
- `meminfo` - Display heap statistics
- `about` - Kernel information
- `test` - Test memory allocator
- **`syscall`** - Test system call interface ⭐ NEW
- **`idtcheck`** - Check IDT integrity
- **`idtinfo`** - Display all IDT entries
- **`funccheck`** - Check function integrity (CRC32)
- `attack` - Simulate IDT hook attack
- `unhook` - Remove IDT hook
- `patch` - Simulate inline hook attack
- `unpatch` - Remove inline hook

### Testing Workflow

**1. Test system calls:** ⭐ NEW
```
syscall     # Tests getpid(), uptime(), write()
```

**2. Verify baseline security:**
```
idtcheck    # Should pass
funccheck   # Should pass
```

**3. Test IDT attack detection:**
```
attack      # Hooks IRQ7 with fake address
idtcheck    # Detects modification (FAILED)
unhook      # Restores original handler
idtcheck    # Verification passes again
```

**4. Test inline hook detection:**
```
patch       # Modifies shell_init() code
funccheck   # Detects CRC32 mismatch (FAILED)
unpatch     # Restores original bytes
funccheck   # Verification passes again
```

See [TESTING.md](TESTING.md) for comprehensive test scenarios and [SYSCALL_IMPLEMENTATION.md](SYSCALL_IMPLEMENTATION.md) for syscall details.

## Architecture

### Memory Layout
- `0x00000000 - 0x000003FF`: Real mode IVT
- `0x00007C00 - 0x00007DFF`: Bootloader (512 bytes)
- `0x00010000 - 0x0001XXXX`: Kernel code/data (~18KB)
- `0x00100000+`: Heap (starts at 1MB)

### Interrupt System
- **ISR 0-31**: CPU exceptions (divide-by-zero, page fault, etc.)
- **IRQ 0-15**: Hardware interrupts (mapped to ISR 32-47)
  - IRQ0: PIT timer (100Hz)
  - IRQ1: Keyboard

### Protected Functions
The KIM monitors these critical functions for modifications:
1. `kernel_main` - Main kernel entry point
2. `irq_handler` - Hardware interrupt dispatcher
3. `fault_handler` - CPU exception handler
4. `shell_init` - Shell initialization

## Implementation Details

### IDT Integrity Checker
- Captures baseline snapshot at boot
- Compares current IDT against baseline
- Validates handler addresses within kernel range
- Detects descriptor modifications

### Function Integrity Checker
- Uses CRC32 (polynomial 0xEDB88320)
- Monitors first 32 bytes of each function
- Detects inline hooks (JMP/CALL patching)
- Supports up to 5 watched functions

### Mock Rootkit Safety
- IDT hook uses unused IRQ7 (doesn't break keyboard)
- Inline hook patches are non-executable
- Full restoration capabilities for cleanup
- Purely educational demonstration

## Project Structure
```
boot/
  boot.asm          # Stage 1 bootloader
kernel/
  kernel.c          # Main kernel entry
  gdt.c/h           # Global Descriptor Table
  idt.c/h           # Interrupt Descriptor Table
  isr.c/h           # Exception/IRQ handlers
  isr_stubs.asm     # Assembly interrupt stubs
  memory.c/h        # Heap allocator
  shell.c/h         # Interactive shell
  kim.c/h           # Kernel Integrity Module
  rootkit.c/h       # Mock attack simulation
drivers/
  vga.c/h           # VGA text mode driver
  keyboard.c/h      # Keyboard driver
  pit.c/h           # Timer driver
linker.ld           # Linker script
Makefile            # Build configuration
```

## Development Notes

### Compiler Flags
```
-m32                    # 32-bit compilation
-ffreestanding          # No standard library
-nostdlib               # No linking with libc
-fno-pie -fno-pic       # Disable position-independent code
-mno-sse -mno-mmx       # Disable SIMD (no SSE setup)
-O0                     # No optimization (for debugging)
```

### Bootloader Constraints
- Reads 40 sectors (20KB) from floppy
- Uses BIOS INT 13h for disk I/O
- Enables A20 line for >1MB memory access
- Switches to protected mode before kernel jump

### Kernel Size Monitoring
Current: ~18KB (36 sectors)
Maximum: 20KB (40 sectors) before bootloader update needed

## Limitations

- Single-core, no multitasking
- No filesystem (direct floppy boot)
- Limited to 20KB kernel size
- No networking or advanced drivers
- Function checksums only cover first 32 bytes
- Maximum 5 monitored functions

## Future Enhancements (Not Implemented)

- System call interface
- User mode processes
- File system support
- Network stack
- Advanced memory protection (paging)
- Real-time rootkit detection daemon

## Educational Purpose

This kernel demonstrates:
- Low-level x86 programming
- Bootloader development
- Interrupt handling
- Protected mode setup
- Rootkit detection techniques
- Security baseline systems

**Not suitable for production use.**

## License

Educational project - use for learning purposes.

## References

- Intel 64 and IA-32 Architectures Software Developer's Manual
- OSDev Wiki: https://wiki.osdev.org/
- Linux Kernel source (for concepts)
- Rootkit detection research papers
