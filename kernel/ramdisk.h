#ifndef RAMDISK_H
#define RAMDISK_H

// If you don't have stdint.h, you might need to define uint32_t, etc. manually.
// For a mini OS, 'int' usually suffices if you aren't strict about bit-width.

#define MAX_FILES 16       // Maximum number of files supported
#define MAX_FILENAME 32    // Max characters in a filename
#define MAX_FILE_SIZE 1024 // 1KB limit per file

// The structure representing a single file in RAM
typedef struct
{
    char name[MAX_FILENAME];
    char content[MAX_FILE_SIZE];
    int size; // Current size of file content
    int used; // 1 if file exists, 0 if slot is empty
} ramdisk_file_t;

// Core RAM Disk Functions
void ramdisk_init();
int rd_create(const char *name);
int rd_read(const char *name, char *buffer, int size);
int rd_write(const char *name, const char *data);
int rd_delete(const char *name);
void rd_list();

#endif