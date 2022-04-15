// Implements __assert_func

#include <revolution.h>
#include <stdio.h>   // snprintf
#include <string.h>  // strrchr
#include "StackTrace.h"

void __assert_func(
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

    char buf[512];
    snprintf(buf, sizeof(buf), "[%s:%i] %s: %s\n%s", filename, line, assertFunc, expr,
            stacktrace);

    // PAL: Turquoise
    const GXColor P_BG = { 0, 67, 67, 255 };
    // NTSC-U: Green
    const GXColor E_BG = { 17, 67, 0, 255 };
    // NTSC-J: Red
    const GXColor J_BG = { 67, 0, 11, 255 };
    // NTSC-K: Purple
    const GXColor K_BG = { 51, 0, 67, 255 };
    // Default: Black
    const GXColor UNK_BG = { 0, 0, 0, 255 };

    GXColor bg = UNK_BG;
    switch (REGION) {
    case REGION_P:
        bg = P_BG;
        break;
    case REGION_E:
        bg = E_BG;
        break;
    case REGION_J:
        bg = J_BG;
        break;
    case REGION_K:
        bg = K_BG;
        break;
    }
    const GXColor fg = { 255, 255, 255, 255 };

    OSFatal(fg, bg, buf);

    __builtin_unreachable();
}
