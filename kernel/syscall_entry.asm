[BITS 32]

global syscall_entry
extern syscall_handler

; System call entry point (INT 0x80)
syscall_entry:
    ; Save all registers
    push ds
    push es
    push fs
    push gs
    pusha
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Build syscall_registers structure on stack
    ; Stack now has: [gs][fs][es][ds][edi][esi][ebp][esp][ebx][edx][ecx][eax]
    ; We need to pass pointer to a struct with: eax, ebx, ecx, edx, esi, edi
    
    ; ESP points to saved EAX, adjust to create struct pointer
    mov eax, esp
    add eax, 28  ; Skip gs, fs, es, ds, edi, esi, ebp (7 * 4 bytes)
    
    ; Build struct on stack in order: eax, ebx, ecx, edx, esi, edi
    push edi     ; edi (arg 5)
    push esi     ; esi (arg 4)
    
    ; Get edx, ecx, ebx, eax from saved context
    mov edx, [esp + 44]  ; saved edx
    mov ecx, [esp + 48]  ; saved ecx
    mov ebx, [esp + 52]  ; saved ebx
    mov eax, [esp + 56]  ; saved eax
    
    push edx     ; edx (arg 3)
    push ecx     ; ecx (arg 2)
    push ebx     ; ebx (arg 1)
    push eax     ; eax (syscall number)
    
    ; Call C handler with pointer to struct
    mov eax, esp
    push eax
    call syscall_handler
    add esp, 4   ; Clean up argument
    
    ; Clean up struct from stack
    add esp, 24  ; 6 values * 4 bytes
    
    ; Restore return value from handler (it modified struct.eax)
    mov eax, [esp + 28]  ; Get modified eax from saved context
    mov [esp + 28], eax  ; Store back
    
    ; Restore registers
    popa
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Return to user mode
    iret
