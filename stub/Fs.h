#pragma once

#include <Common.h>

enum {
    FS_PERM_READ = 1 << 0,
    FS_PERM_WRITE = 1 << 1,
};

typedef struct {
    u32 size;
    u32 position;
} FsFileStats;

bool Fs_init(void);

bool Fs_createFile(const char *path, u8 attrs, u8 ownerPerms, u8 groupPerms, u8 otherPerms);

bool Fs_rename(const char *srcPath, const char *dstPath);

bool Fs_deinit(void);
