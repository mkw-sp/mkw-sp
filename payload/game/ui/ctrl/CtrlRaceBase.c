#include "CtrlRaceBase.h"

#include "game/system/SaveManager.h"

#include <sp/settings/ClientSettings.h>

void CtrlRaceBase_initLabelVisibility(CtrlRaceBase *this, u32 localPlayerCount, const char *pane) {
    if (localPlayerCount <= 2) {
        if (SaveManager_GetHUDLabels() == kHudLabels_Show) {
            return;
        }
    }

    LayoutUIControl_setPaneVisible(this, pane, false);
}
