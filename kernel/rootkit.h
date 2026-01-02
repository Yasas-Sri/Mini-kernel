#ifndef ROOTKIT_H
#define ROOTKIT_H

#include <stdint.h>

// Mock rootkit functions for educational demonstration

// IDT-based attacks
void rootkit_hook_keyboard();
void rootkit_unhook();
int rootkit_is_active();

// Inline hook attacks (function patching)
void rootkit_patch_function();
void rootkit_unpatch_function();
int rootkit_inline_hook_active();

#endif
