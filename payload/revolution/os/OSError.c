#include "revolution/nwc24/NWC24Utils.h"
#include "revolution/os.h"

#include <game/system/Console.h>
#include <sp/StackTrace.h>
#include <sp/storage/LogFile.h>

#include <stdio.h>
#include <string.h>

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

    panic("Invalid caller");
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

__attribute__((noreturn)) REPLACE void OSPanic(const char *UNUSED(filename), int UNUSED(lineNumber),
        const char *message, ...) {
    char messageFormat[256];
    NWC24iStrLCpy(messageFormat, message, sizeof(messageFormat));
    if (messageFormat[strlen(messageFormat) - 1] == '\n') {
        messageFormat[strlen(messageFormat) - 1] = '\0';
    }

    char panicMessage[256];
    va_list args;
    va_start(args, message);
    vsnprintf(panicMessage, sizeof(panicMessage), messageFormat, args);
    va_end(args);

    panic(panicMessage);
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
