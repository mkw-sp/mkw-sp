#include <string.h>

size_t strlcpy(char *dst, const char *src, size_t size) {
    size_t i = 0;
    if (size > 0) {
        for (; i < size - 1 && *src; i++) {
            *dst++ = *src++;
        }
        *dst = '\0';
    }
    return i + strlen(src);
}
