#ifndef PROMPT_H
#define PROMPT_H

#include <stdint.h>

// Prompt styles
#define PROMPT_STYLE_SIMPLE     0
#define PROMPT_STYLE_COLORFUL   1
#define PROMPT_STYLE_MINIMAL    2
#define PROMPT_STYLE_HACKER     3

void prompt_init();
void prompt_print();
void prompt_set_style(uint8_t style);
void prompt_set_user(const char* username);
void prompt_set_path(const char* path);

#endif
