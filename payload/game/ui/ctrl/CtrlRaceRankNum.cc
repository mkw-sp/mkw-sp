#include "CtrlRaceRankNum.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/Page.hh"

namespace UI {

void CtrlRaceRankNum::calcSelf() {
    setVisible(!isDisabled());

    REPLACED(calcSelf)();
}

bool CtrlRaceRankNum::vf_48() {
    if (isDisabled()) {
        return true;
    }

    return REPLACED(vf_48)();
}

bool CtrlRaceRankNum::isDisabled() const {
    auto pageId = getPage()->id();
    if (pageId != PageId::TARace && pageId != PageId::GhostReplayRace) {
        return false;
    }

    if (System::RaceConfig::Instance()->raceScenario().playerCount <= 1) {
        return true;
    }

    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::RankControl>();
    return setting != SP::ClientSettings::RankControl::Always;
}

} // namespace UI
