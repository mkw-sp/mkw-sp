#include <string.h>

#include <Common.h>

__attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))
void *memset(void *s, int c, size_t n) {
    u8 *p = s;
    for (size_t i = 0; i < n; i++) {
        p[i] = c;
    }
    return s;
}
