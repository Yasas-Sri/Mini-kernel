[BITS 16]
[ORG 0x7C00]

start:
    ; Setup segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Force boot drive to 0 (Floppy) for now
    mov byte [boot_drive], 0

    ; Print boot message
    mov si, boot_msg
    call print_string

    ; Load kernel from disk
    ; Reset disk
    mov ah, 0x00
    mov dl, [boot_drive]
    int 0x13

    ; Read kernel sectors
    mov ax, 0x1000          ; Segment to load into
    mov es, ax
    xor bx, bx              ; Offset 0 (so address is 0x1000:0x0000 = 0x10000)
    
    mov ah, 0x02            ; Read sectors function
    mov al, 110             ; Read 110 sectors (55KB - room for growth)
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Start from sector 2 (sector 1 is boot sector)
    mov dh, 0               ; Head 0
    mov dl, [boot_drive]    ; Drive number
    int 0x13
    jc disk_error

    mov si, load_success
    call print_string

    ; Enable A20 line
    call enable_a20

    ; Load GDT
    cli
    lgdt [gdt_descriptor]

    ; Switch to protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to 32-bit code
    jmp 0x08:protected_mode

disk_error:
    mov si, disk_error_msg
    call print_string
    hlt
    jmp $

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

print_string:
    pusha
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0
    int 0x10
    jmp .loop
.done:
    popa
    ret

boot_msg db 'MiniKernel-Sec Bootloader v0.1', 13, 10, 0
load_success db 'Kernel loaded successfully!', 13, 10, 0
disk_error_msg db 'ERROR: Disk read failed!', 13, 10, 0
boot_drive db 0

; GDT Setup
align 4
gdt_start:
    ; Null descriptor
    dq 0x0000000000000000

gdt_code:
    dw 0xFFFF       ; Limit low
    dw 0x0000       ; Base low
    db 0x00         ; Base middle
    db 10011010b    ; Access byte (present, ring 0, code, executable, readable)
    db 11001111b    ; Flags + Limit high
    db 0x00         ; Base high

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b    ; Access byte (present, ring 0, data, writable)
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
protected_mode:
    ; Setup segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Jump to kernel at 0x10000
    jmp 0x10000

    ; Should never reach here
    hlt
    jmp $

times 510-($-$$) db 0
dw 0xAA55