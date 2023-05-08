#include <Common.hh>

#include <cstdarg>
#include <cstdio>

namespace nw4r::db {

REPLACE void Panic(const char * /* filename */, int /* lineNumber */, const char *format, ...) {
    char panicMessage[256];

    va_list args;
    va_start(args, format);
    vsnprintf(panicMessage, sizeof(panicMessage), format, args);
    va_end(args);

    panic(panicMessage);
}

} // namespace nw4r::db
