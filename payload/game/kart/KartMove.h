#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x04 - 0x00];
    s16 timers[6];
    u8 _10[0x24 - 0x10];
} KartBoost;
static_assert(sizeof(KartBoost) == 0x24);

typedef struct {
    u8 _000[0x020 - 0x000];
    f32 internalSpeed;
    u8 _024[0x02c - 0x024];
    f32 hardSpeedLimit;
    u8 _030[0x074 - 0x030];
    Vec3 internalVelDir;
    u8 _080[0x0fe - 0x080];
    u16 mtCharge;
    u16 smtCharge;
    u8 _102[0x108 - 0x102];
    KartBoost boost;
    u8 _12c[0x14c - 0x12c];
    u16 ssmtCharge;
    u8 _14e[0x294 - 0x14e];
} KartMove;
static_assert(sizeof(KartMove) == 0x294);
