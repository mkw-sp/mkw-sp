// Resources:
// - https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/HW/WII_IPC.cpp
// - https://wiibrew.org/wiki/Hardware/IPC
// - https://wiibrew.org/wiki/IOS

#include "Ios.h"

#include "Cache.h"

#include <stdalign.h>
#include <string.h>

static volatile u32 *ppcmsg = (u32 *)0xcd000000;
static volatile u32 *ppcctrl = (u32 *)0xcd000004;
static volatile u32 *armmsg = (u32 *)0xcd000008;

enum {
    X1  = 1 << 0,
    Y2  = 1 << 1,
    Y1  = 1 << 2,
    X2  = 1 << 3,
    IY1 = 1 << 4,
    IY2 = 1 << 5,
};

enum {
    CMD_OPEN = 1,
    CMD_CLOSE = 2,
    CMD_IOCTL = 6,
    CMD_IOCTLV = 7,
};

typedef struct {
    u32 cmd;
    s32 result;
    s32 fd;
    union {
        struct {
            u32 path;
            u32 mode;
        } open;
        struct {
            u32 ioctl;
            u32 input;
            u32 inputSize;
            u32 output;
            u32 outputSize;
        } ioctl;
        struct {
            u32 ioctlv;
            u32 inputCount;
            u32 outputCount;
            u32 pairs;
        } ioctlv;
    };
    u8 user[0x40 - 0x20];
} Request;
static_assert(sizeof(Request) == 0x40);

static alignas(0x20) Request request;

static void sync(void) {
    DCFlushRange(&request, sizeof(request));

    *ppcmsg = VIRTUAL_TO_PHYSICAL(&request);
    *ppcctrl = X1;

    while ((*ppcctrl & Y2) != Y2);
    *ppcctrl = Y2;

    u32 reply;
    do {
        while ((*ppcctrl & Y1) != Y1);
        reply = *armmsg;
        *ppcctrl = Y1;

        *ppcctrl = X2;
    } while (reply != VIRTUAL_TO_PHYSICAL(&request));

    DCInvalidateRange(&request, sizeof(request));
}

void Ios_init(void) {
    //while ((*ppcctrl & Y2) != Y2);
    *ppcctrl = Y2;
    *ppcctrl = Y1;
}

s32 Ios_open(const char *path, u32 mode) {
    DCFlushRange((void *)path, strlen(path) + 1);

    memset(&request, 0, sizeof(request));

    request.cmd = CMD_OPEN;
    request.fd = 0;
    request.open.path = VIRTUAL_TO_PHYSICAL(path);
    request.open.mode = mode;

    sync();

    return request.result;
}

s32 Ios_close(s32 fd) {
    memset(&request, 0, sizeof(request));

    request.cmd = CMD_CLOSE;
    request.fd = fd;

    sync();

    return request.result;
}

s32 Ios_ioctl(s32 fd, u32 ioctl, const void *input, u32 inputSize, void *output, u32 outputSize) {
    DCFlushRange((void *)input, inputSize);
    DCFlushRange(output, outputSize);

    memset(&request, 0, sizeof(request));

    request.cmd = CMD_IOCTL;
    request.fd = fd;
    request.ioctl.ioctl = ioctl;
    request.ioctl.input = VIRTUAL_TO_PHYSICAL(input);
    request.ioctl.inputSize = inputSize;
    request.ioctl.output = VIRTUAL_TO_PHYSICAL(output);
    request.ioctl.outputSize = outputSize;

    sync();

    DCInvalidateRange(output, outputSize);

    return request.result;
}

s32 Ios_ioctlv(s32 fd, u32 ioctlv, u32 inputCount, u32 outputCount, IoctlvPair *pairs) {
    for (u32 i = 0; i < inputCount + outputCount; i++) {
        if (pairs[i].data) {
            DCFlushRange(pairs[i].data, pairs[i].size);
            pairs[i].data = (void *)VIRTUAL_TO_PHYSICAL(pairs[i].data);
        }
    }
    DCFlushRange(pairs, (inputCount + outputCount) * sizeof(IoctlvPair));

    memset(&request, 0, sizeof(request));

    request.cmd = CMD_IOCTLV;
    request.fd = fd;
    request.ioctlv.ioctlv = ioctlv;
    request.ioctlv.inputCount = inputCount;
    request.ioctlv.outputCount = outputCount;
    request.ioctlv.pairs = VIRTUAL_TO_PHYSICAL(pairs);

    sync();

    for (u32 i = 0; i < inputCount + outputCount; i++) {
        if (pairs[i].data) {
            pairs[i].data = PHYSICAL_TO_VIRTUAL((u32)pairs[i].data);
            DCInvalidateRange(pairs[i].data, pairs[i].size);
        }
    }

    return request.result;
}
