#include "CtrlRaceTime.h"

#include "../../system/SaveManager.h"

s8 CtrlRaceTime_getPlayerId(CtrlRaceTime *this);

PATCH_B(CtrlRaceTime_getPlayerId, CtrlRaceBase_getPlayerId);

void CtrlRaceTime_initLabelVisibility(CtrlRaceTime *this, u32 localPlayerCount) {
    CtrlRaceBase_initLabelVisibility(this, localPlayerCount, "set_p");
}
