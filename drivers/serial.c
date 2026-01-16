#include "serial.h"

static int serial_enabled = 0;

// Port I/O functions
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void serial_init() {
    outb(SERIAL_COM1 + 1, 0x00);    // Disable all interrupts
    outb(SERIAL_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(SERIAL_COM1 + 1, 0x00);    //                  (hi byte)
    outb(SERIAL_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    
    serial_enabled = 1;
}

static int is_transmit_empty() {
    return inb(SERIAL_COM1 + 5) & 0x20;
}

void serial_putchar(char c) {
    if (!serial_enabled) return;
    
    while (is_transmit_empty() == 0);
    
    outb(SERIAL_COM1, c);
}

void serial_write_string(const char* str) {
    if (!serial_enabled) return;
    
    while (*str) {
        serial_putchar(*str);
        str++;
    }
}

int serial_is_enabled() {
    return serial_enabled;
}
