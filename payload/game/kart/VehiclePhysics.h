#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x074 - 0x000];
    Vec3 externalVel;
    u8 _080[0x0b0 - 0x080];
    Vec3 movingRoadVel;
    u8 _0bc[0x0c8 - 0x0bc];
    Vec3 movingWaterVel;
    u8 _0d4[0x14c - 0x0d4];
    Vec3 internalVel;
    u8 _158[0x1b4 - 0x158];
} VehiclePhysics;
static_assert(sizeof(VehiclePhysics) == 0x1b4);
