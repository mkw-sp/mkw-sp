#pragma once

#include "KartMove.h"
#include "KartState.h"
#include "VehiclePhysics.h"

#include "../race/Driver.h"

#include "../snd/KartSound.h"

enum {
    COLLISION_TYPE_FLOOR = 1 << 0,
};

// TODO move
typedef struct {
    u32 types;
    u8 _04[0x6c - 0x04];
    u32 closestFloorFlags;
    u32 closestFloorSettings;
    u8 _74[0x84 - 0x74];
} CollisionInfo;
static_assert(sizeof(CollisionInfo) == 0x84);

typedef struct {
    u8 _00[0x04 - 0x00];
    KartState *state;
    u8 _08[0x14 - 0x08];
    Driver *driver;
    u8 _18[0x1c - 0x18];
    KartSound *sound;
    u8 _20[0x64 - 0x20];
} KartAccessor;
static_assert(sizeof(KartAccessor) == 0x64);

typedef struct {
    KartAccessor *accessor;
    u8 _0[0xc - 0x4];
} KartObjectProxy;
static_assert(sizeof(KartObjectProxy) == 0xc);

Vec3 *KartObjectProxy_getPos(KartObjectProxy *this);

Vec3 *KartObjectProxy_getLastPos(KartObjectProxy *this);

u16 KartObjectProxy_getTireCount(KartObjectProxy *this);

VehiclePhysics *KartObjectProxy_getVehiclePhysics(KartObjectProxy *this);

KartMove *KartObjectProxy_getKartMove(KartObjectProxy *this);

CollisionInfo *KartObjectProxy_getBodyCollisionInfo(KartObjectProxy *this);

CollisionInfo *KartObjectProxy_getTireCollisionInfo(KartObjectProxy *this, u16 i);

f32 KartObjectProxy_getInternalSpeed(KartObjectProxy *this);
