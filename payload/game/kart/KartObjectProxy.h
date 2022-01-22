#pragma once

#include "KartMove.h"
#include "VehiclePhysics.h"

typedef struct {
    u8 _0[0xc - 0x0];
} KartObjectProxy;
static_assert(sizeof(KartObjectProxy) == 0xc);

VehiclePhysics *KartObjectProxy_getVehiclePhysics(KartObjectProxy *this);

KartMove *KartObjectProxy_getKartMove(KartObjectProxy *this);

f32 KartObjectProxy_getInternalSpeed(KartObjectProxy *this);
