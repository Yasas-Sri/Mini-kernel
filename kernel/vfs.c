#include "vfs.h"
#include "ramdisk.h"
#include "string.h"
#include "../drivers/vga.h" // <--- INCLUDE THIS

void vfs_init()
{
    ramdisk_init();

    // Create default system files
    rd_create("os.conf");
    rd_write("os.conf", "theme=dark;user=admin;");

    rd_create("readme.txt");
    rd_write("readme.txt", "Welcome to MiniOS!");
}

int vfs_create_file(const char *filename)
{
    return rd_create(filename);
}

int vfs_read_file(const char *filename, char *buffer, int size)
{
    return rd_read(filename, buffer, size);
}

int vfs_write_file(const char *filename, const char *data)
{
    return rd_write(filename, data);
}

int vfs_delete_file(const char *filename)
{
    return rd_delete(filename);
}

void vfs_list_files()
{
    rd_list();
}

void vfs_load_config()
{
    char buffer[128];
    int bytes = vfs_read_file("os.conf", buffer, 128);

    if (bytes > 0)
    {
        vga_write_string("[System] Loading configuration...\n");
        vga_write_string("Config found: ");
        vga_write_string(buffer);
        vga_write_string("\n");

        // Parse configuration settings
        if (strstr(buffer, "theme=dark"))
        {
            vga_apply_dark_theme();
            vga_write_string("[Config] Dark theme enabled\n");
        }
        else if (strstr(buffer, "theme=light"))
        {
            vga_apply_light_theme();
            vga_write_string("[Config] Light theme enabled\n");
        }

        // Parse user setting
        char *user_pos = strstr(buffer, "user=");
        if (user_pos)
        {
            vga_write_string("[Config] User: ");
            // Find the username after "user="
            user_pos += 5; // Skip "user="
            char username[32];
            int i = 0;
            while (user_pos[i] != ';' && user_pos[i] != '\0' && i < 31)
            {
                username[i] = user_pos[i];
                i++;
            }
            username[i] = '\0';
            vga_write_string(username);
            vga_write_string("\n");
        }
    }
    else
    {
        vga_write_string("[System] Warning: os.conf not found.\n");
    }
}