[GLOBAL isr0]
[GLOBAL isr1]
[GLOBAL isr2]
[GLOBAL isr3]
[GLOBAL isr4]
[GLOBAL isr5]
[GLOBAL isr6]
[GLOBAL isr7]
[GLOBAL isr8]
[GLOBAL isr9]
[GLOBAL isr10]
[GLOBAL isr11]
[GLOBAL isr12]
[GLOBAL isr13]
[GLOBAL isr14]
[GLOBAL isr15]
[GLOBAL isr16]
[GLOBAL isr17]
[GLOBAL isr18]
[GLOBAL isr19]
[GLOBAL isr20]
[GLOBAL isr21]
[GLOBAL isr22]
[GLOBAL isr23]
[GLOBAL isr24]
[GLOBAL isr25]
[GLOBAL isr26]
[GLOBAL isr27]
[GLOBAL isr28]
[GLOBAL isr29]
[GLOBAL isr30]
[GLOBAL isr31]

[GLOBAL irq0]
[GLOBAL irq1]
[GLOBAL irq2]
[GLOBAL irq3]
[GLOBAL irq4]
[GLOBAL irq5]
[GLOBAL irq6]
[GLOBAL irq7]
[GLOBAL irq8]
[GLOBAL irq9]
[GLOBAL irq10]
[GLOBAL irq11]
[GLOBAL irq12]
[GLOBAL irq13]
[GLOBAL irq14]
[GLOBAL irq15]

[EXTERN fault_handler]
[EXTERN irq_handler]

; Exception handlers (ISR 0-31)
isr0:
    cli
    push dword 0
    call fault_handler
    add esp, 4
    sti
    iret

isr1:
    cli
    push dword 1
    call fault_handler
    add esp, 4
    sti
    iret

isr2:
    cli
    push dword 2
    call fault_handler
    add esp, 4
    sti
    iret

isr3:
    cli
    push dword 3
    call fault_handler
    add esp, 4
    sti
    iret

isr4:
    cli
    push dword 4
    call fault_handler
    add esp, 4
    sti
    iret

isr5:
    cli
    push dword 5
    call fault_handler
    add esp, 4
    sti
    iret

isr6:
    cli
    push dword 6
    call fault_handler
    add esp, 4
    sti
    iret

isr7:
    cli
    push dword 7
    call fault_handler
    add esp, 4
    sti
    iret

isr8:
    cli
    push dword 8
    call fault_handler
    add esp, 4
    sti
    iret

isr9:
    cli
    push dword 9
    call fault_handler
    add esp, 4
    sti
    iret

isr10:
    cli
    push dword 10
    call fault_handler
    add esp, 4
    sti
    iret

isr11:
    cli
    push dword 11
    call fault_handler
    add esp, 4
    sti
    iret

isr12:
    cli
    push dword 12
    call fault_handler
    add esp, 4
    sti
    iret

isr13:
    cli
    push dword 13
    call fault_handler
    add esp, 4
    sti
    iret

isr14:
    cli
    push dword 14
    call fault_handler
    add esp, 4
    sti
    iret

isr15:
    cli
    push dword 15
    call fault_handler
    add esp, 4
    sti
    iret

isr16:
    cli
    push dword 16
    call fault_handler
    add esp, 4
    sti
    iret

isr17:
    cli
    push dword 17
    call fault_handler
    add esp, 4
    sti
    iret

isr18:
    cli
    push dword 18
    call fault_handler
    add esp, 4
    sti
    iret

isr19:
    cli
    push dword 19
    call fault_handler
    add esp, 4
    sti
    iret

isr20:
    cli
    push dword 20
    call fault_handler
    add esp, 4
    sti
    iret

isr21:
    cli
    push dword 21
    call fault_handler
    add esp, 4
    sti
    iret

isr22:
    cli
    push dword 22
    call fault_handler
    add esp, 4
    sti
    iret

isr23:
    cli
    push dword 23
    call fault_handler
    add esp, 4
    sti
    iret

isr24:
    cli
    push dword 24
    call fault_handler
    add esp, 4
    sti
    iret

isr25:
    cli
    push dword 25
    call fault_handler
    add esp, 4
    sti
    iret

isr26:
    cli
    push dword 26
    call fault_handler
    add esp, 4
    sti
    iret

isr27:
    cli
    push dword 27
    call fault_handler
    add esp, 4
    sti
    iret

isr28:
    cli
    push dword 28
    call fault_handler
    add esp, 4
    sti
    iret

isr29:
    cli
    push dword 29
    call fault_handler
    add esp, 4
    sti
    iret

isr30:
    cli
    push dword 30
    call fault_handler
    add esp, 4
    sti
    iret

isr31:
    cli
    push dword 31
    call fault_handler
    add esp, 4
    sti
    iret

; IRQ handlers (IRQ 0-15)
irq0:
    cli
    push dword 0
    call irq_handler
    add esp, 4
    sti
    iret

irq1:
    cli
    push dword 1
    call irq_handler
    add esp, 4
    sti
    iret

irq2:
    cli
    push dword 2
    call irq_handler
    add esp, 4
    sti
    iret

irq3:
    cli
    push dword 3
    call irq_handler
    add esp, 4
    sti
    iret

irq4:
    cli
    push dword 4
    call irq_handler
    add esp, 4
    sti
    iret

irq5:
    cli
    push dword 5
    call irq_handler
    add esp, 4
    sti
    iret

irq6:
    cli
    push dword 6
    call irq_handler
    add esp, 4
    sti
    iret

irq7:
    cli
    push dword 7
    call irq_handler
    add esp, 4
    sti
    iret

irq8:
    cli
    push dword 8
    call irq_handler
    add esp, 4
    sti
    iret

irq9:
    cli
    push dword 9
    call irq_handler
    add esp, 4
    sti
    iret

irq10:
    cli
    push dword 10
    call irq_handler
    add esp, 4
    sti
    iret

irq11:
    cli
    push dword 11
    call irq_handler
    add esp, 4
    sti
    iret

irq12:
    cli
    push dword 12
    call irq_handler
    add esp, 4
    sti
    iret

irq13:
    cli
    push dword 13
    call irq_handler
    add esp, 4
    sti
    iret

irq14:
    cli
    push dword 14
    call irq_handler
    add esp, 4
    sti
    iret

irq15:
    cli
    push dword 15
    call irq_handler
    add esp, 4
    sti
    iret
