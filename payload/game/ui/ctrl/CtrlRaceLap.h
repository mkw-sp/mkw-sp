#pragma once

#include "CtrlRaceBase.h"

typedef struct {
    CtrlRaceBase;
    u8 _198[0x19c - 0x198];
} CtrlRaceLap;
static_assert(sizeof(CtrlRaceLap) == 0x19c);
