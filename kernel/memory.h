#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define HEAP_START 0x100000 // Start heap at 1MB
#define HEAP_SIZE 0x100000  // 1MB heap size

typedef struct memory_block
{
    size_t size;
    int is_free;
    struct memory_block *next;
} memory_block_t;

void memory_init();
void *kmalloc(size_t size);
void kfree(void *ptr);
void memory_info();

#endif