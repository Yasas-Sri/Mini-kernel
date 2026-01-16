#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include <stdint.h>

// Progress bar
void ui_progress_bar(const char* label, uint32_t current, uint32_t total);

// Spinner animation frames
void ui_spinner_start(const char* message);
void ui_spinner_update();
void ui_spinner_stop();

// Status indicators
void ui_status_success(const char* message);
void ui_status_error(const char* message);
void ui_status_warning(const char* message);
void ui_status_info(const char* message);

// Horizontal separator
void ui_separator(char style);  // '-', '=', '─', etc.

// Box drawing
void ui_box_start(const char* title);
void ui_box_line(const char* content);
void ui_box_end();

#endif
