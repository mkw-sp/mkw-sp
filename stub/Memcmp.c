#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n) {
    const char *t1 = s1, *t2 = s2;

    for (size_t i = 0; i < n; i++) {
        if (t1[i] < t2[i]) {
            return -1;
        }

        if (t1[i] > t2[i]) {
            return 1;
        }
    }

    return 0;
}
