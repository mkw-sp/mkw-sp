#pragma once

#include <revolution.h>

enum {
    RK_NAND_RESULT_OK = 0x0,
    RK_NAND_RESULT_BUSY = 0x2,
    RK_NAND_RESULT_ACCESS = 0x3,
    RK_NAND_RESULT_NOEXISTS = 0x4,
    RK_NAND_RESULT_NOSPACE = 0x7,
    RK_NAND_RESULT_OTHER = 0x8,
};
