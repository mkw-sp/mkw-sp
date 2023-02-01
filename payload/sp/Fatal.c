// Implements __assert_func

#include <revolution.h>
#include <stdio.h>   // snprintf
#include <string.h>  // strrchr
#include "StackTrace.h"

__attribute__((noreturn)) void __assert_func(
        const char *filename, int line, const char *assertFunc, const char *expr) {
    {
        const char *search = strrchr(filename, '\\');
        if (search != NULL) {
            filename = search + 1;
        }
    }

    char stacktrace[384];
    size_t l = WriteStackTraceShort(stacktrace, sizeof(stacktrace), OSGetStackPointer());
    if (l <= 0) {
        snprintf(stacktrace, sizeof(stacktrace), "Stacktrace is unrecoverable.");
    }

    panic("[%s:%i] %s: %s\n\n" "%s", filename, line, assertFunc, expr, stacktrace);
}
