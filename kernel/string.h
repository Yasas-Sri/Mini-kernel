// kernel/string.h
#ifndef STRING_H
#define STRING_H

int strcmp(const char *s1, const char *s2);
void strcpy(char *dest, const char *src);
int strlen(const char *str);
char *strstr(const char *haystack, const char *needle);
void *memcpy(void *dest, const void *src, int n);

#endif