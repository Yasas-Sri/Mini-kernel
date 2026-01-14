#include "ramdisk.h"
#include "string.h"         // Use your global string.h
#include "../drivers/vga.h" // <--- INCLUDE THIS

// The actual "Disk" in memory
ramdisk_file_t files[MAX_FILES];

void ramdisk_init()
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        files[i].used = 0;
        files[i].size = 0;
        files[i].name[0] = 0;
    }
}

int rd_create(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && strcmp(files[i].name, name) == 0)
        {
            return -1;
        }
    }
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (!files[i].used)
        {
            strcpy(files[i].name, name);
            files[i].used = 1;
            files[i].size = 0;
            files[i].content[0] = 0;
            return 0;
        }
    }
    return -2;
}

int rd_write(const char *name, const char *data)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && strcmp(files[i].name, name) == 0)
        {
            int len = strlen(data);
            if (len >= MAX_FILE_SIZE)
                len = MAX_FILE_SIZE - 1;
            strcpy(files[i].content, data);
            files[i].size = len;
            return len;
        }
    }
    return -1;
}

int rd_read(const char *name, char *buffer, int size)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && strcmp(files[i].name, name) == 0)
        {
            int copy_len = files[i].size;
            if (copy_len > size)
                copy_len = size;

            for (int j = 0; j < copy_len; j++)
            {
                buffer[j] = files[i].content[j];
            }
            buffer[copy_len] = 0;
            return copy_len;
        }
    }
    return -1;
}

int rd_delete(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && strcmp(files[i].name, name) == 0)
        {
            files[i].used = 0;
            files[i].size = 0;
            return 0;
        }
    }
    return -1;
}

void rd_list()
{
    vga_write_string("\n--- File System List ---\n"); // <--- CHANGED
    int found = 0;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used)
        {
            vga_write_string("File: ");      // <--- CHANGED
            vga_write_string(files[i].name); // <--- CHANGED
            vga_write_string(" | Size: ");   // <--- CHANGED
            vga_write_dec(files[i].size);    // <--- CHANGED: Print actual size
            vga_write_string(" bytes\n");    // <--- CHANGED
            found = 1;
        }
    }
    if (!found)
        vga_write_string("(Empty)\n");              // <--- CHANGED
    vga_write_string("------------------------\n"); // <--- CHANGED
}