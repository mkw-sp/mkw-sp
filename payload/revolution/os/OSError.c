#include "revolution/os.h"

#include "sp/LogFile.h"
#include "sp/StackTrace.h"

#include "game/system/Console.h"

#include <stdarg.h>
#include <stdio.h>

static const char *GetPrefix(BinaryType bin) {
    switch (bin) {
    case BINARY_DOL:
        return "&e[OSReport] ";
    case BINARY_REL:
        return "&e[REL] ";
    case BINARY_SP:
        return "&6[SP] &c";
    case BINARY_ILLEGAL:
        return "&8[?] &7";
    }

    assert(!"Invalid caller");
    __builtin_unreachable();
}

static inline BinaryType ClassifyCaller(void *sp) {
    StackTraceIterator it;
    StackTraceIterator_create(&it, sp);

    void *addr = NULL;
    {
        const bool res = StackTraceIterator_read(&it, &addr);
        assert(res);
    }
    // We need to escape my_OSReport (assume this function is inlined)
    assert(ClassifyPointer(addr) == BINARY_SP);
    {
        const bool res = StackTraceIterator_read(&it, &addr);
        assert(res);
    }
    return ClassifyPointer(addr);
}

void my_OSReport(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    va_start(args, msg);
    LogFile_vprintf(msg, args);
    va_end(args);

    {
        char formatted[128];
        va_start(args, msg);
        vsnprintf(formatted, sizeof(formatted), msg, args);
        va_end(args);

        const char* prefix = GetPrefix(ClassifyCaller(OSGetStackPointer()));

        char buf[128];
        const size_t len = snprintf(buf, sizeof(buf), "%s%s",
                prefix, formatted);

        Console_addLine(buf, len);
    }
}
PATCH_B(OSReport, my_OSReport);
