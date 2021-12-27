#pragma once

#include "CtrlRaceBase.h"

typedef struct {
    CtrlRaceBase;
} CtrlRaceSpeed;

CtrlRaceSpeed *CtrlRaceSpeed_ct(CtrlRaceSpeed *this);

void CtrlRaceSpeed_load(CtrlRaceSpeed *this, const char *variant, u32 localPlayerId);
