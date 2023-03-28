#include <Common.h>

wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        s[i] = c;
    }

    return s;
}
