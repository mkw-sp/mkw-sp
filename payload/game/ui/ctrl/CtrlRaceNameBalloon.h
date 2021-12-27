#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x188 - 0x000];
} CtrlRaceNameBalloon;

typedef struct {
    u8 _00[0x03 - 0x00];
    u8 localPlayerId;
    u32 nameCount;
    u8 _08[0x14 - 0x08];
    s32 playerIds[12];
    bool nameIsEnabled[12]; // Moved
    u8 _50[0xc4 - 0x50];
    Vec3 namePositions[12]; // Moved
} BalloonManager;

BalloonManager *BalloonManager_ct(BalloonManager *this);

void BalloonManager_dt(BalloonManager *this, s32 type);

void BalloonManager_init(BalloonManager *this, u8 localPlayerId);

void BalloonManager_addNameControl(BalloonManager *this, CtrlRaceNameBalloon *nameControl);
