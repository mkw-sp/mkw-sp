#pragma once

#include <Common.h>

typedef struct {
    u8 _0[0xc - 0x0];
} KartObjectProxy;

f32 KartObjectProxy_getSpeed(KartObjectProxy *this);
