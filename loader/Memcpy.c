#include <string.h>

#include <Common.h>

__attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))
void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    u8 *d = dest;
    const u8 *s = src;

    while (n-- > 0) {
        *d++ = *s++;
    }

    return dest;
}
