#ifndef PAGER_H
#define PAGER_H

// Pager for displaying long text with pausing
void pager_display(const char* text);
void pager_display_lines(const char** lines, int num_lines);

#endif
