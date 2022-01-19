#include "CtrlRaceLap.h"

#include "../../system/SaveManager.h"

void CtrlRaceLap_initLabelVisibility(CtrlRaceLap *this, u32 localPlayerCount) {
    CtrlRaceBase_initLabelVisibility(this, localPlayerCount, "lap_text");
}
