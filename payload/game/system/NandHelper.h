#pragma once

#include <revolution.h>

enum {
    RK_NAND_RESULT_OK = 0x0,
    RK_NAND_RESULT_BUSY = 0x2,
    RK_NAND_RESULT_ACCESS = 0x3,
    RK_NAND_RESULT_NOEXISTS = 0x4,
    RK_NAND_RESULT_OTHER = 0x8,
};

u32 NandHelper_open(const char *path, NANDFileInfo *info, u8 accType);

u32 NandHelper_close(NANDFileInfo *info);

u32 NandHelper_read(NANDFileInfo *info, void *buf, u32 length, u32 offset);

u32 NandHelper_getLength(NANDFileInfo *info, u32 *length);

u32 NandHelper_readFile(const char *path, void *buf, u32 maxLength);

u32 NandHelper_readFromFile(const char *path, void *buf, u32 length, u32 offset);

u32 NandHelper_readDir(const char *path, char *nameList, u32 *num);

u32 NandHelper_createDir(const char *path, u8 perm);

u32 NandHelper_getHomeDir(char *path);

u32 NandHelper_getType(const char *path, u32 *type);
