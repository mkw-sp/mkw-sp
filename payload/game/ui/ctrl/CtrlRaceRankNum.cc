#include "CtrlRaceRankNum.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/Page.hh"

namespace UI {

void CtrlRaceRankNum::calcSelf() {
    auto pageId = getPage()->id();
    if (pageId == PageId::TARace || pageId == PageId::GhostReplayRace) {
        bool visible = System::RaceConfig::Instance()->raceScenario().playerCount > 1;
        auto *saveManager = System::SaveManager::Instance();
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::RankControl>();
        if (setting != SP::ClientSettings::RankControl::Always) {
            visible = false;
        }
        setVisible(visible);
    }

    REPLACED(calcSelf)();
}

} // namespace UI
