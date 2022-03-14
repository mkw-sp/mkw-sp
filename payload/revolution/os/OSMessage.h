#pragma once

#include <Common.h>

typedef void *OSMessage;

typedef struct {
    u8 _00[0x20 - 0x00];
} OSMessageQueue;
static_assert(sizeof(OSMessageQueue) == 0x20);

enum {
    OS_MESSAGE_NOBLOCK = 0,
    OS_MESSAGE_BLOCK = 1,
};

void OSInitMessageQueue(OSMessageQueue *mq, OSMessage *msgArray, s32 msgCount);
BOOL OSSendMessage(OSMessageQueue *mq, OSMessage msg, s32 flags);
BOOL OSReceiveMessage(OSMessageQueue *mq, OSMessage *msg, s32 flags);
