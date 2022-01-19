#include "CtrlRaceTime.h"

#include "../../system/SaveManager.h"

void CtrlRaceTime_initLabelVisibility(CtrlRaceTime *this, u32 localPlayerCount) {
    CtrlRaceBase_initLabelVisibility(this, localPlayerCount, "set_p");
}
