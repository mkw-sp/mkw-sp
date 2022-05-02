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
    NAND_ACCESS_WRITE = 0x2,
};

enum {
    NAND_SEEK_SET = 0,
};

enum {
    NAND_MAX_PATH = 64,
    NAND_MAX_NAME = 12,
};

typedef struct {
    u8 _00[0x8c - 0x00];
} NANDFileInfo;
static_assert(sizeof(NANDFileInfo) == 0x8c);

s32 NANDOpen(const char *path, NANDFileInfo *info, u8 accType);

s32 NANDClose(NANDFileInfo *info);

s32 NANDRead(NANDFileInfo *info, void *buf, u32 length);

s32 NANDSeek(NANDFileInfo *info, s32 offset, s32 whence);

s32 NANDReadDir(const char *path, char *nameList, u32 *num);

s32 NANDGetHomeDir(char path[NAND_MAX_PATH]);

s32 NANDMove(const char *path, const char *destDir);
