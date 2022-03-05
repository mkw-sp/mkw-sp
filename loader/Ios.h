#pragma once

#include <Common.h>

typedef struct {
    void *data;
    u32 size;
} IoctlvPair;

s32 Ios_open(const char *path, u32 mode);

s32 Ios_close(s32 fd);

s32 Ios_ioctl(s32 fd, u32 ioctl, const void *input, u32 inputSize, void *output, u32 outputSize);

s32 Ios_ioctlv(s32 fd, u32 ioctlv, u32 inputCount, u32 outputCount, IoctlvPair *pairs);

void Ios_ioctlvReboot(s32 fd, u32 ioctlv, u32 inputCount, IoctlvPair *pairs);
