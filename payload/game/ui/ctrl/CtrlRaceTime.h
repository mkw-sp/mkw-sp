#pragma once

#include "CtrlRaceBase.h"

typedef struct {
    CtrlRaceBase;
    u8 _198[0x1c4 - 0x198];
} CtrlRaceTime;
static_assert(sizeof(CtrlRaceTime) == 0x1c4);
