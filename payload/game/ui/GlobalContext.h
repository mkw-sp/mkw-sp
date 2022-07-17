#pragma once

#include "MiiGroup.h"

#include "../system/GhostFile.h"

typedef struct {
    u8 _000[0x12c - 0x000];
    u32 timeAttackCharacterId;
    u8 _130[0x13c - 0x130];
    u32 timeAttackVehicleId;
    u8 _140[0x188 - 0x140];
    MiiGroup playerMiis;
    u8 _220[0x238 - 0x220];
    MiiGroup localPlayerMiis;
    u8 _2d0[0x3bc - 0x2d0];
    u32 _3bc;
    u8 _3c0[0x3c4-0x3c0];
    u32 timeAttackGhostType;
    s32 timeAttackCourseId;
    s32 timeAttackLicenseId;
    u8 _3d0[0x510 - 0x3d0];
    u32 timeAttackGhostCount; // Added
    u32 timeAttackGhostIndices[11]; // Added
} GlobalContext;
static_assert(offsetof(GlobalContext, timeAttackGhostCount) == 0x510);

void GlobalContext_copyPlayerMiis(GlobalContext *this);

void GlobalContext_changeLicense(GlobalContext *this);
