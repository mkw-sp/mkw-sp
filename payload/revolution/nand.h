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
    NAND_PERM_OTHER_READ = 0x01,
    NAND_PERM_OTHER_WRITE = 0x02,
    NAND_PERM_GROUP_READ = 0x04,
    NAND_PERM_GROUP_WRITE = 0x08,
    NAND_PERM_OWNER_READ = 0x10,
    NAND_PERM_OWNER_WRITE = 0x20,
    NAND_PERM_OTHER_MASK = NAND_PERM_OTHER_WRITE | NAND_PERM_OTHER_READ,
    NAND_PERM_GROUP_MASK = NAND_PERM_GROUP_WRITE | NAND_PERM_GROUP_READ,
    NAND_PERM_OWNER_MASK = NAND_PERM_OWNER_WRITE | NAND_PERM_OWNER_READ,
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

s32 NANDPrivateCreate(const char *path, u8 perm, u8 attr);

s32 NANDOpen(const char *path, NANDFileInfo *info, u8 accType);
s32 NANDPrivateOpen(const char *path, NANDFileInfo *info, u8 accType);

s32 NANDClose(NANDFileInfo *info);

s32 NANDRead(NANDFileInfo *info, void *buf, u32 length);

s32 NANDWrite(NANDFileInfo *info, const void *buf, u32 length);

s32 NANDSeek(NANDFileInfo *info, s32 offset, s32 whence);

s32 NANDPrivateCreateDir(const char *path, u8 perm, u8 attr);

s32 NANDGetHomeDir(char path[NAND_MAX_PATH]);

s32 NANDPrivateDelete(const char *path);

s32 NANDMove(const char *path, const char *destDir);
s32 NANDPrivateMove(const char *path, const char *destDir);
