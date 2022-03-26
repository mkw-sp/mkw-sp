#include <wchar.h>

int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] < s2[i]) {
            return -1;
        }

        if (s1[i] > s2[i]) {
            return 1;
        }

        if (s1[i] == L'\0') {
            return 0;
        }
    }

    return 0;
}

wchar_t *wcsrchr(const wchar_t *wcs, wchar_t wc) {
    wchar_t *res = NULL;

    while (*wcs != L'\0') {
        if (*wcs == wc) {
            res = wcs;
        }

        wcs++;
    }

    return res;
}
