#include <stdarg.h>
#include <stdio.h>
#include "game/system/Console.h"
#include "revolution/os.h"
#include "sp/StackTrace.h"
#include "sp/storage/LogFile.h"

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
    LogFile_VPrintf(msg, args);
    va_end(args);

    const char *prefix = GetPrefix(ClassifyCaller(OSGetStackPointer()));
    va_start(args, msg);
    Console_vprintf(prefix, msg, args);
    va_end(args);
}
PATCH_B(OSReport, my_OSReport);
