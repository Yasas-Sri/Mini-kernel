/*
#include <stdint.h>
#include "timer.h"

volatile uint32_t timer_ticks = 0;
uint32_t PIT_FREQUENCY = 0;  // Hz

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void timer_init(uint32_t frequency)
{
    PIT_FREQUENCY = frequency;

    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);          // Command port
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void timer_tick()
{
    timer_ticks++;
}

uint32_t timer_get_ticks()
{
    return timer_ticks;
}
*/

#include <stdint.h>
#include "timer.h"
#include "sysinfo.h" // if you want to update system stats

volatile uint32_t timer_ticks = 0;
uint32_t PIT_FREQUENCY = 0; // Hz

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Initialize PIT timer
void timer_init(uint32_t frequency)
{
    PIT_FREQUENCY = frequency;

    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);                  // Command port: channel 0, low/high, mode 3
    outb(0x40, divisor & 0xFF);        // Low byte
    outb(0x40, (divisor >> 8) & 0xFF); // High byte
}

// Called from timer interrupt
void timer_tick()
{
    timer_ticks++;
    // Update uptime (assuming 1000Hz timer = 1ms per tick)
    if (PIT_FREQUENCY > 0) {
        uint32_t ms_per_tick = 1000 / PIT_FREQUENCY;
        sysinfo_update_uptime(ms_per_tick);
    }
}

uint32_t timer_get_ticks()
{
    return timer_ticks;
}
