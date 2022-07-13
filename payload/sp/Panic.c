#include "Panic.h"

#include <revolution/os.h>

#include <stdarg.h>
#include <stdio.h>

void panic(const char* format, ...)
{
    char message_format[128];
    snprintf(message_format, sizeof(message_format), "MKW-SP v%s\n\n" "%s", versionInfo.name, format);

    va_list args;
    va_start(args, format);
    {
        char message[512];
        vsnprintf(message, sizeof(message), message_format, args);

        const GXColor foreground = GXCOLOR_WHITE;
        GXColor background;
        switch (REGION)
        {
            case REGION_J: { background = GXCOLOR_RED  ; break; }
            case REGION_E: { background = GXCOLOR_BLUE ; break; }
            case REGION_P: { background = GXCOLOR_GREEN; break; }
            case REGION_K: { background = GXCOLOR_PINK ; break; }
            default      : { background = GXCOLOR_BLACK; break; }
        }

        OSFatal(foreground, background, message);
        __builtin_unreachable();
    }
    va_end(args);
}