
#pragma once

#include <Common.h>

s32 IOS_Open(const char *path, u32 flags);
s32 IOS_Close(s32 handle);
s32 IOS_Ioctl(s32 handle, s32 command, void *input, u32 inputSize, void *output,
              u32 outputSize);