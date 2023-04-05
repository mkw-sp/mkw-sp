#pragma once

#include <Common.h>

#define NET_SHA1_DIGEST_SIZE 0x14

u32 NETCalcCRC32(const void *datap, u32 size);

typedef struct {
    u8 _00[0x60 - 0x00];
} NETSHA1Context;
static_assert(sizeof(NETSHA1Context) == 0x60);

void NETSHA1Init(NETSHA1Context *context);
void NETSHA1Update(NETSHA1Context *context, const void *input, u32 length);
void NETSHA1GetDigest(NETSHA1Context *context, void *digest);
