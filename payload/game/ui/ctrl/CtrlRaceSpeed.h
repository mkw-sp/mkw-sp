#pragma once

#include "CtrlRaceBase.h"

typedef struct {
    CtrlRaceBase;
} CtrlRaceSpeed;

CtrlRaceSpeed *CtrlRaceSpeed_ct(CtrlRaceSpeed *this);

void CtrlRaceSpeed_load(CtrlRaceSpeed *this, u32 localPlayerCount, u32 localPlayerId);
