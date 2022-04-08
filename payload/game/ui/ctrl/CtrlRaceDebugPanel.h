#pragma once

#include "game/ui/UIControl.h"

typedef struct {
    LayoutUIControl;
} CtrlRaceDebugPanel;

CtrlRaceDebugPanel *CtrlRaceDebugPanel_ct(CtrlRaceDebugPanel *self);

void CtrlRaceDebugPanel_load(CtrlRaceDebugPanel *self);
