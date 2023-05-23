extern "C" {
#include "Panic.h"
#include "StackTrace.h"

#include <revolution/os.h>
}

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void panic(const char *format, ...) {
    char messageFormat[288];
    snprintf(messageFormat, sizeof(messageFormat),
            "MKW-SP v%s\n\n"
            "%s\n\n",
            versionInfo.name, format);

    char message[512] = "???";

    int size = -1;
    {
        va_list args;
        va_start(args, format);
        size = vsnprintf(message, sizeof(message), messageFormat, args);
        va_end(args);
    }

    if (size >= 0) {
        WriteStackTraceShort(message + size, sizeof(message) - size, OSGetStackPointer());
    }

    GXColor background;
    switch (REGION) {
    case REGION_J:
        background = GXCOLOR_RED;
        break;
    case REGION_E:
        background = GXCOLOR_BLUE;
        break;
    case REGION_P:
        background = GXCOLOR_GREEN;
        break;
    case REGION_K:
        background = GXCOLOR_PINK;
        break;
    default:
        background = GXCOLOR_BLACK;
        break;
    }

    // Impromptu luminosity calculation
    GXColor foreground = GXCOLOR_WHITE;
    if (background.r * background.r + background.g * background.g + background.b * background.b >
            (128 * 128) * 3) {
        foreground = GXCOLOR_BLACK;
    }

    OSFatal(foreground, background, message);
}
