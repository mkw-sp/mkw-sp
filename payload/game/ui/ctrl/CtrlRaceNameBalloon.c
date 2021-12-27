#include "CtrlRaceNameBalloon.h"

static BalloonManager *my_BalloonManager_ct(BalloonManager *this) {
    this->nameCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->namePositions); i++) {
        this->namePositions[i] = (Vec3) { 0 };
    }

    return this;
}

PATCH_B(BalloonManager_ct, my_BalloonManager_ct);

static void my_BalloonManager_dt(BalloonManager *this, s32 type) {
    if (type > 0) {
        delete(this);
    }
}

PATCH_B(BalloonManager_dt, my_BalloonManager_dt);

static void my_BalloonManager_init(BalloonManager *this, u8 localPlayerId) {
    this->localPlayerId = localPlayerId;
    for (u32 i = 0; i < ARRAY_SIZE(this->playerIds); i++) {
        this->playerIds[i] = -1;
    }
    for (u32 i = 0; i < ARRAY_SIZE(this->nameIsEnabled); i++) {
        this->nameIsEnabled[i] = false;
    }
}

PATCH_B(BalloonManager_init, my_BalloonManager_init);

static void my_BalloonManager_addNameControl(BalloonManager *this, CtrlRaceNameBalloon *nameControl) {
    UNUSED(nameControl);

    this->nameCount++;
}

PATCH_B(BalloonManager_addNameControl, my_BalloonManager_addNameControl);
