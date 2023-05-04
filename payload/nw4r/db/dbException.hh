#pragma once

#include "dbConsole.hh"

namespace nw4r::db {

enum {
    EXCEPTION_INFO_NONE = (0 << 0),
    EXCEPTION_INFO_MAIN = (1 << 0),
    EXCEPTION_INFO_GPR = (1 << 1),
    EXCEPTION_INFO_GPRMAP = (1 << 2),
    EXCEPTION_INFO_SRR0MAP = (1 << 3),
    EXCEPTION_INFO_FPR = (1 << 4),
    EXCEPTION_INFO_TRACE = (1 << 5),

    EXCPETION_INFO_DEFAULT = EXCEPTION_INFO_MAIN | EXCEPTION_INFO_TRACE,
    EXCEPTION_INFO_ALL = EXCEPTION_INFO_MAIN | EXCEPTION_INFO_GPR | EXCEPTION_INFO_GPRMAP |
            EXCEPTION_INFO_SRR0MAP | EXCEPTION_INFO_FPR | EXCEPTION_INFO_TRACE
};

typedef bool (*ExceptionUserCallback)(ConsoleHandle consoleHandle, void *arg);

} // namespace nw4r::db
