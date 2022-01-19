#include "CtrlRaceBase.h"

#include "../../system/SaveManager.h"

void CtrlRaceBase_initLabelVisibility(CtrlRaceBase *this, u32 localPlayerCount, const char *pane) {
    if (localPlayerCount <= 2) {
        if (SaveManager_getSettingHudLabels(s_saveManager) == SP_SETTING_HUD_LABELS_SHOW) {
            return;
        }
    }

    LayoutUIControl_setPaneVisible(this, pane, false);
}
