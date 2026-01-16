#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SERIAL_COM1 0x3F8

void serial_init();
void serial_putchar(char c);
void serial_write_string(const char* str);
int serial_is_enabled();

#endif
