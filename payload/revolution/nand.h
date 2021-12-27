#pragma once

#include <Common.h>

enum {
    NAND_RESULT_OK = 0,
    NAND_RESULT_ACCESS = -1,
    NAND_RESULT_ALLOC_FAILED = -2,
    NAND_RESULT_BUSY = -3,
    NAND_RESULT_NOEXISTS = -12,
    NAND_RESULT_FATAL_ERROR = -128,
};

enum {
    NAND_PERM_OWNER_READ = 0x10,
    NAND_PERM_OWNER_WRITE = 0x20,
};

enum {
    NAND_ACCESS_READ = 0x1,
};

enum {
    NAND_MAX_PATH = 64,
    NAND_MAX_NAME = 12,
};

enum {
    NAND_TYPE_FILE = 0x1,
    NAND_TYPE_DIR = 0x2,
};

typedef struct {
    u8 _00[0x8c - 0x00];
} NANDFileInfo;

s32 NANDReadDir(const char *path, char *nameList, u32 *num);

s32 NANDGetHomeDir(char path[NAND_MAX_PATH]);
