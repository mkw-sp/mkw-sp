#include <string.h>

size_t strlen(const char *s) {
    const char *f = s;
    while (*s != '\0') {
        s++;
    }
    return s - f;
}
