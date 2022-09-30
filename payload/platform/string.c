#include <Common.h>
#include <string.h>

#include <revolution/net/NETMisc.h>

void *memcpy_slow(void *dst, const void *src, size_t n);
void *memset_slow(void *dst, int c, size_t n);

// Use the optimized versions of the functions
PATCH_B(memcpy_slow, memcpy);
PATCH_B(memset_slow, memset);

// Correct the return values of the functions
void *memcpy(void *dst, const void *src, size_t n) {
    NETMemCpy(dst, src, n);
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    NETMemSet(dst, c, n);
    return dst;
}

// Based on https://github.com/llvm/llvm-project/blob/main/libc/src/string/strrchr.cpp#L15
char *strrchr(const char *src, int c) {
    const char ch = c;
    char *last_occurrence = NULL;
    for (; *src; ++src) {
        if (*src == ch)
            last_occurrence = (char *)(src);
    }
    return last_occurrence;
}

// Based on https://github.com/llvm/llvm-project/blob/main/libc/src/string/strstr.cpp#L18
static char *llvm_strstr(const char *haystack, const char *needle) {
    for (size_t i = 0; haystack[i]; ++i) {
        size_t j;
        for (j = 0; haystack[i + j] && haystack[i + j] == needle[j]; ++j)
            ;
        if (!needle[j])
            return (char *)(haystack + i);
    }
    return NULL;
}

// Many ugly crashes caused by this function. Ideally, we'll catch them before these
// asserts trigger.
static char *my_strstr(char *string, char *substring) {
    assert(string);
    assert(substring);

    return llvm_strstr(string, substring);
}
PATCH_B(strstr, my_strstr);
