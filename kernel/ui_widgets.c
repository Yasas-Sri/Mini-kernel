#include "ui_widgets.h"
#include "../drivers/vga.h"

static const char spinner_frames[] = {'|', '/', '-', '\\'};
static uint8_t spinner_index = 0;

// String length helper
static int strlen_local(const char *str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void ui_progress_bar(const char* label, uint32_t current, uint32_t total) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    // Print label
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string(label);
    vga_write_string(" [");
    
    // Calculate percentage
    uint32_t percent = (current * 100) / total;
    uint32_t filled = (current * 40) / total;  // 40 chars wide
    
    // Draw filled portion
    vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
    for (uint32_t i = 0; i < filled; i++) {
        vga_write_string("█");
    }
    
    // Draw empty portion
    vga_set_color(VGA_COLOR_DARK_GREY, bg);
    for (uint32_t i = filled; i < 40; i++) {
        vga_write_string("░");
    }
    
    // Print percentage
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string("] ");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, bg);
    char percent_str[8];
    percent_str[0] = '0' + (percent / 10);
    percent_str[1] = '0' + (percent % 10);
    percent_str[2] = '%';
    percent_str[3] = '\0';
    vga_write_string(percent_str);
    
    vga_write_string("\n");
    vga_set_color(fg, bg);
}

void ui_spinner_start(const char* message) {
    spinner_index = 0;
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, bg);
    vga_putchar(spinner_frames[spinner_index]);
    vga_write_string(" ");
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string(message);
    vga_set_color(fg, bg);
}

void ui_spinner_update() {
    spinner_index = (spinner_index + 1) % 4;
    // In a real implementation, you'd move cursor back and update
    // For now, just note the next frame is ready
}

void ui_spinner_stop() {
    vga_write_string(" [DONE]\n");
}

void ui_status_success(const char* message) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, bg);
    vga_write_string(" ✓ ");
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string(message);
    vga_write_string("\n");
    vga_set_color(fg, bg);
}

void ui_status_error(const char* message) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    vga_set_color(VGA_COLOR_LIGHT_RED, bg);
    vga_write_string(" ✗ ");
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string(message);
    vga_write_string("\n");
    vga_set_color(fg, bg);
}

void ui_status_warning(const char* message) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    vga_set_color(VGA_COLOR_YELLOW, bg);
    vga_write_string(" ⚠ ");
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string(message);
    vga_write_string("\n");
    vga_set_color(fg, bg);
}

void ui_status_info(const char* message) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    vga_set_color(VGA_COLOR_LIGHT_BLUE, bg);
    vga_write_string(" ℹ ");
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string(message);
    vga_write_string("\n");
    vga_set_color(fg, bg);
}

void ui_separator(char style) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    vga_set_color(VGA_COLOR_DARK_GREY, bg);
    for (int i = 0; i < 80; i++) {
        vga_putchar(style);
    }
    vga_write_string("\n");
    vga_set_color(fg, bg);
}

void ui_box_start(const char* title) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    int title_len = strlen_local(title);
    int padding = (76 - title_len) / 2;
    
    vga_set_color(VGA_COLOR_DARK_GREY, bg);
    vga_write_string("╔");
    for (int i = 0; i < 78; i++) vga_write_string("═");
    vga_write_string("╗\n");
    
    vga_write_string("║");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, bg);
    for (int i = 0; i < padding; i++) vga_write_string(" ");
    vga_write_string(title);
    for (int i = 0; i < (76 - title_len - padding); i++) vga_write_string(" ");
    vga_set_color(VGA_COLOR_DARK_GREY, bg);
    vga_write_string(" ║\n");
    
    vga_write_string("╠");
    for (int i = 0; i < 78; i++) vga_write_string("═");
    vga_write_string("╣\n");
    
    vga_set_color(fg, bg);
}

void ui_box_line(const char* content) {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    int content_len = strlen_local(content);
    
    vga_set_color(VGA_COLOR_DARK_GREY, bg);
    vga_write_string("║ ");
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, bg);
    vga_write_string(content);
    
    for (int i = content_len; i < 76; i++) vga_write_string(" ");
    
    vga_set_color(VGA_COLOR_DARK_GREY, bg);
    vga_write_string(" ║\n");
    
    vga_set_color(fg, bg);
}

void ui_box_end() {
    uint8_t fg, bg;
    extern void vga_get_theme_colors(uint8_t *fg, uint8_t *bg);
    vga_get_theme_colors(&fg, &bg);
    
    vga_set_color(VGA_COLOR_DARK_GREY, bg);
    vga_write_string("╚");
    for (int i = 0; i < 78; i++) vga_write_string("═");
    vga_write_string("╝\n");
    
    vga_set_color(fg, bg);
}
