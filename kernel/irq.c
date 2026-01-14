#include "irq.h"
#include "timer.h"
#include "sysinfo.h"

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void irq_handler_c(uint8_t irq)
{
    if (irq == 0) // PIT timer
    {
        timer_tick(); // increment ticks
        sysinfo_increment_interrupt(0);

        if (PIT_FREQUENCY > 0)
            sysinfo_update_uptime(1000 / PIT_FREQUENCY);
    }
    else if (irq == 1) // Keyboard
    {
        sysinfo_increment_interrupt(1);
    }

    // Send EOI
    outb(0x20, 0x20);
}
