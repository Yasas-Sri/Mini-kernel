#include "memory.h"
#include "../drivers/vga.h"

static memory_block_t *heap_start = NULL;
static uint32_t used_memory = 0;

void memory_init()
{
    heap_start = (memory_block_t *)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(memory_block_t);
    heap_start->is_free = 1;
    heap_start->next = NULL;

    vga_write_string("[*] Initializing Memory Manager...\n");
    vga_write_string("[+] Heap Start: 0x100000\n");
    vga_write_string("[+] Heap Size: 1MB\n");
}

void *kmalloc(size_t size)
{
    if (size == 0)
        return NULL;

    // Align to 4 bytes
    size = (size + 3) & ~3;

    memory_block_t *current = heap_start;

    while (current != NULL)
    {
        if (current->is_free && current->size >= size)
        {
            // Found a suitable block
            if (current->size >= size + sizeof(memory_block_t) + 4)
            {
                // Split the block
                memory_block_t *new_block = (memory_block_t *)((uint8_t *)current + sizeof(memory_block_t) + size);
                new_block->size = current->size - size - sizeof(memory_block_t);
                new_block->is_free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = 0;
            used_memory += size + sizeof(memory_block_t);

            return (void *)((uint8_t *)current + sizeof(memory_block_t));
        }
        current = current->next;
    }

    return NULL; // Out of memory
}

void kfree(void *ptr)
{
    if (ptr == NULL)
        return;

    memory_block_t *block = (memory_block_t *)((uint8_t *)ptr - sizeof(memory_block_t));
    block->is_free = 1;
    used_memory -= block->size + sizeof(memory_block_t);

    // Merge with next block if free
    if (block->next != NULL && block->next->is_free)
    {
        block->size += sizeof(memory_block_t) + block->next->size;
        block->next = block->next->next;
    }

    // Merge with previous block if free
    memory_block_t *current = heap_start;
    while (current != NULL && current->next != block)
    {
        current = current->next;
    }

    if (current != NULL && current->is_free)
    {
        current->size += sizeof(memory_block_t) + block->size;
        current->next = block->next;
    }
}

void memory_info()
{
    vga_write_string("\n=== Memory Information ===\n");
    vga_write_string("Total Memory: 1MB\n");
    vga_write_string("Used Memory: ");
    // Simple display - you can enhance with proper number-to-string conversion
    if (used_memory > 1024)
    {
        vga_write_string("~KB\n");
    }
    else
    {
        vga_write_string("~bytes\n");
    }
    vga_write_string("Free Memory: ");
    vga_write_string("Available\n\n");
}