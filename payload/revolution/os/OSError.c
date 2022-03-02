#include "revolution/os.h"

#include "sp/LogFile.h"

#include <stdarg.h>
#include <stdio.h>

void my_OSReport(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    va_start(args, msg);
    LogFile_vprintf(msg, args);
    va_end(args);
}
PATCH_B(OSReport, my_OSReport);
