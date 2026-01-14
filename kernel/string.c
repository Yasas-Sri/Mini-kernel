// Add these to kernel/kernel.c or a new kernel/string.c

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void strcpy(char *dest, const char *src)
{
    while ((*dest++ = *src++))
        ;
}

int strlen(const char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return len;
}

char *strstr(const char *haystack, const char *needle)
{
    if (!*needle)
        return (char *)haystack;
    for (; *haystack; haystack++)
    {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }
        if (!*n)
            return (char *)haystack;
    }
    return 0; // NULL
}

void *memcpy(void *dest, const void *src, int n)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (int i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
    return dest;
}