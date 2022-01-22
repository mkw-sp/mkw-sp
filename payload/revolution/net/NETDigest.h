#pragma once

#include <Common.h>

u32 NETCalcCRC32(const void *datap, u32 size);

typedef struct {
    u8 _00[0x60 - 0x00];
} NETSHA1Context;
static_assert(sizeof(NETSHA1Context) == 0x60);

void NETSHA1Init(NETSHA1Context *context);
void NETSHA1Update(NETSHA1Context *context, const void *input, u32 length);
void NETSHA1GetDigest(NETSHA1Context *context, void *digest);

inline static void NETCalcSHA1(void *digest, const void *input, u32 length)
{
    NETSHA1Context context;

    NETSHA1Init(&context);
    NETSHA1Update(&context, input, length);
    NETSHA1GetDigest(&context, digest);
}
