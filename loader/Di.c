// Resources:
// - https://wiibrew.org/wiki//dev/di
// - https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/IOS/DI/DI.cpp
// - https://github.com/Chadderz121/brainslug-wii/blob/master/src/di/di.c

#include "Di.h"

#include <common/Ios.h>

#include <stdalign.h>

enum {
    IOCTL_READ_DISK_ID = 0x70,
    IOCTL_READ = 0x71,
    IOCTL_GET_COVER_STATUS = 0x88,
    IOCTL_RESET = 0x8a,
    IOCTLV_OPEN_PARTITION = 0x8b,
    IOCTL_READ_UNENCRYPTED = 0x8d,
};

enum {
    RESULT_SUCCESS = 1,
};

static alignas(0x20) const char path[] = "/dev/di";
static alignas(0x20) u32 in[8];
static alignas(0x20) u32 out[8];
static alignas(0x20) IoctlvPair pairs[5];
static alignas(0x20) u8 tmd[0x49e4];
static s32 fd = -1;

bool Di_init(void) {
    fd = Ios_open(path, 0);
    return fd >= 0;
}

bool Di_readDiskId(void) {
    in[0] = IOCTL_READ_DISK_ID << 24;

    s32 result = Ios_ioctl(fd, IOCTL_READ_DISK_ID, in, sizeof(in), (void *)0x80000000, 0x20);

    return result == RESULT_SUCCESS;
}

bool Di_readUnencrypted(void *dst, u32 size, u32 offset) {
    in[0] = IOCTL_READ_UNENCRYPTED << 24;
    in[1] = size;
    in[2] = offset >> 2;

    s32 result = Ios_ioctl(fd, IOCTL_READ_UNENCRYPTED, in, sizeof(in), dst, size);

    return result == RESULT_SUCCESS;
}

bool Di_openPartition(u32 offset) {
    in[0] = IOCTLV_OPEN_PARTITION << 24;
    in[1] = offset >> 2;

    pairs[0].data = in;
    pairs[0].size = sizeof(in);
    pairs[1].data = NULL;
    pairs[1].size = 0;
    pairs[2].data = NULL;
    pairs[2].size = 0;
    pairs[3].data = tmd;
    pairs[3].size = sizeof(tmd);
    pairs[4].data = out;
    pairs[4].size = sizeof(out);

    s32 result = Ios_ioctlv(fd, IOCTLV_OPEN_PARTITION, 3, 2, pairs);

    return result == RESULT_SUCCESS;
}

bool Di_read(void *dst, u32 size, u32 offset) {
    in[0] = IOCTL_READ << 24;
    in[1] = size;
    in[2] = offset >> 2;

    s32 result = Ios_ioctl(fd, IOCTL_READ, in, sizeof(in), dst, size);

    return result == RESULT_SUCCESS;
}

bool Di_isInserted(void) {
    in[0] = IOCTL_GET_COVER_STATUS << 24;

    s32 result = Ios_ioctl(fd, IOCTL_GET_COVER_STATUS, in, sizeof(in), out, sizeof(out));

    if (result != RESULT_SUCCESS) {
        return false;
    }

    return out[0] == 2;
}

bool Di_reset(void) {
    in[0] = IOCTL_RESET << 24;
    in[1] = true; // Enable spinup

    s32 result = Ios_ioctl(fd, IOCTL_RESET, in, sizeof(in), NULL, 0);

    return result == RESULT_SUCCESS;
}

bool Di_deinit(void) {
    s32 result = Ios_close(fd);

    fd = -1;

    return result == 0;
}
