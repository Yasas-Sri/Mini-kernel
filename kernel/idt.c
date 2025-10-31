#include "idt.h"

struct idt_entry idt_entries[256];
struct idt_ptr idt_ptr_struct;

extern void idt_flush(uint32_t);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = selector;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

void idt_init()
{
    idt_ptr_struct.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr_struct.base = (uint32_t)&idt_entries;

    // Clear all IDT entries
    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, 0, 0, 0);
    }

    // Remap PIC
    // ICW1
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x11), "Nd"((uint16_t)0x20));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x11), "Nd"((uint16_t)0xA0));

    // ICW2
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x20), "Nd"((uint16_t)0x21));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x28), "Nd"((uint16_t)0xA1));

    // ICW3
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x04), "Nd"((uint16_t)0x21));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x02), "Nd"((uint16_t)0xA1));

    // ICW4
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x01), "Nd"((uint16_t)0x21));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x01), "Nd"((uint16_t)0xA1));

    // Mask all interrupts
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x0), "Nd"((uint16_t)0x21));
    __asm__ volatile("outb %0, %1" : : "a"((uint8_t)0x0), "Nd"((uint16_t)0xA1));

    idt_flush((uint32_t)&idt_ptr_struct);
}