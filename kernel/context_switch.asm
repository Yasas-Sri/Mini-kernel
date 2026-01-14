[BITS 32]

; void context_switch(cpu_context_t *old_context, cpu_context_t *new_context)
; Switches CPU context between two processes
; Structure offsets (all 32-bit unless noted):
; 0:eax 4:ebx 8:ecx 12:edx 16:esi 20:edi 24:ebp 28:esp
; 32:eip 36:eflags 40:cr3 44:cs(16) 46:ds(16) 48:es(16) 50:fs(16) 52:gs(16) 54:ss(16)

global context_switch

context_switch:
    cli                     ; Disable interrupts during switch
    
    mov eax, [esp + 4]      ; old_context
    mov edx, [esp + 8]      ; new_context
    
    ; If old_context is NULL, just load new context
    test eax, eax
    jz .restore
    
    ; Save current process context
    mov [eax + 4], ebx
    mov [eax + 8], ecx
    mov [eax + 16], esi
    mov [eax + 20], edi
    mov [eax + 24], ebp
    mov [eax + 28], esp
    
    ; Save return address as the instruction to resume at
    mov ecx, [esp]
    mov [eax + 32], ecx     ; Save EIP
    
    ; Save flags
    pushfd
    pop ecx
    mov [eax + 36], ecx
    
    ; Save data segments
    mov cx, ds
    mov [eax + 46], cx
    mov cx, es  
    mov [eax + 48], cx
    mov cx, ss
    mov [eax + 54], cx
    
.restore:
    ; Restore new process context
    mov ebx, [edx + 4]
    mov ecx, [edx + 8]
    mov esi, [edx + 16]
    mov edi, [edx + 20]
    mov ebp, [edx + 24]
    mov esp, [edx + 28]
    
    ; Restore segments
    mov ax, [edx + 46]
    mov ds, ax
    mov ax, [edx + 48]
    mov es, ax
    mov ax, [edx + 54]
    mov ss, ax
    
    ; Restore flags
    push dword [edx + 36]
    popfd
    
    ; Jump to the saved EIP
    push dword [edx + 32]
    sti                     ; Re-enable interrupts
    ret                     ; Jump to saved EIP



