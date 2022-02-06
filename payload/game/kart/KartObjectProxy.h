#pragma once

#include "KartMove.h"
#include "KartState.h"
#include "VehiclePhysics.h"

typedef struct {
    u8 _00[0x04 - 0x00];
    KartState *state;
    u8 _08[0x64 - 0x08];
} KartAccessor;
static_assert(sizeof(KartAccessor) == 0x64);

typedef struct {
    KartAccessor *accessor;
    u8 _0[0xc - 0x4];
} KartObjectProxy;
static_assert(sizeof(KartObjectProxy) == 0xc);

Vec3 *KartObjectProxy_getPos(KartObjectProxy *this);

Vec3 *KartObjectProxy_getLastPos(KartObjectProxy *this);

VehiclePhysics *KartObjectProxy_getVehiclePhysics(KartObjectProxy *this);

KartMove *KartObjectProxy_getKartMove(KartObjectProxy *this);

f32 KartObjectProxy_getInternalSpeed(KartObjectProxy *this);
