#include "Fs.h"

#include <common/Ios.h>

#include <stdalign.h>
#include <string.h>

enum {
    IOCTL_RENAME = 0x8,
    IOCTL_CREATE_FILE = 0x9,
    IOCTL_GET_FILE_STATS = 0xb,
};

static alignas(0x20) const char path[] = "/dev/fs";
static s32 fd = -1;

bool Fs_init(void) {
    fd = Ios_open(path, 0);
    return fd >= 0;
}

bool Fs_createFile(const char *path, u8 attrs, u8 ownerPerms, u8 groupPerms, u8 otherPerms) {
    alignas(0x20) u8 in[0x4c];

    if (strlcpy((char *)(in + 0x6), path, 0x40) >= 0x40) {
        return false;
    }
    in[0x46] = ownerPerms;
    in[0x47] = groupPerms;
    in[0x48] = otherPerms;
    in[0x49] = attrs;

    return Ios_ioctl(fd, IOCTL_CREATE_FILE, in, sizeof(in), NULL, 0) == 0;
}

bool Fs_rename(const char *srcPath, const char *dstPath) {
    alignas(0x20) u8 in[0x80];

    if (strlcpy((char *)(in + 0x0), srcPath, 0x40) >= 0x40) {
        return false;
    }
    if (strlcpy((char *)(in + 0x40), dstPath, 0x40) >= 0x40) {
        return false;
    }

    return Ios_ioctl(fd, IOCTL_RENAME, in, sizeof(in), NULL, 0) == 0;
}

bool Fs_getFileStats(s32 fd, FsFileStats *fileStats) {
    alignas(0x20) u32 out[2];

    if (Ios_ioctl(fd, IOCTL_GET_FILE_STATS, NULL, 0, out, sizeof(out)) != 0) {
        return false;
    }

    fileStats->size = out[0];
    fileStats->position = out[1];
    return true;
}

bool Fs_deinit(void) {
    s32 result = Ios_close(fd);

    fd = -1;

    return result == 0;
}
