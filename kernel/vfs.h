#ifndef VFS_H
#define VFS_H

// Initialize the VFS and the underlying RAM disk
void vfs_init();

// Standard file operations
int vfs_create_file(const char *filename);
int vfs_read_file(const char *filename, char *buffer, int size);
int vfs_write_file(const char *filename, const char *data);
int vfs_delete_file(const char *filename);
void vfs_list_files();

// Configuration helper
void vfs_load_config();

#endif