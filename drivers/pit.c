#include "pit.h"
#include "vga.h"

static volatile uint32_t timer_ticks = 0;


static inline void __outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void pit_init(uint32_t frequency)
{
    uint32_t divisor = PIT_FREQUENCY / frequency;

    
    __outb(PIT_COMMAND, 0x36);


    __outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    __outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

    vga_write_string("[*] Initializing Programmable Interval Timer (PIT)...\n");
    vga_write_string("[+] PIT configured for ");
    if (frequency == 100) {
        vga_write_string("100Hz (10ms intervals)\n");
    } else {
        vga_write_string("custom frequency\n");
    }
}

void pit_handler()
{
    timer_ticks++;
  
}

uint32_t pit_get_ticks()
{
    return timer_ticks;
}
