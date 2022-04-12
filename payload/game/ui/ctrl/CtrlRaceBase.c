#include "CtrlRaceBase.h"

#include "../../system/SaveManager.h"

void CtrlRaceBase_initLabelVisibility(CtrlRaceBase *this, u32 localPlayerCount, const char *pane) {
    if (localPlayerCount <= 2) {
        if (SaveManager_getSetting(s_saveManager, kSetting_HudLabels) == kHudLabels_Show) {
            return;
        }
    }

    LayoutUIControl_setPaneVisible(this, pane, false);
}
