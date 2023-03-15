#include "CtrlRaceBase.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/TeamColors.hh"
#include "game/ui/page/RacePage.hh"

namespace UI {

CtrlRaceBase::CtrlRaceBase() = default;

CtrlRaceBase::~CtrlRaceBase() = default;

void CtrlRaceBase::setPaneColor(const char *paneName, bool teamColors) {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.localPlayerCount < 2) {
        teamColors = true;
    }

    if (teamColors) {
        auto *saveManager = System::SaveManager::Instance();
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::HUDTeamColors>();
        if (setting == SP::ClientSettings::HUDTeamColors::Disable) {
            teamColors = false;
        }
    }

    if (raceScenario.spMaxTeamSize < 2 || !teamColors) {
        REPLACED(setPaneColor)(paneName, teamColors);
        return;
    }

    u32 playerId = getPlayerId();
    auto *pane = m_mainLayout.findPaneByName(paneName);
    assert(pane);
    for (size_t i = 0; i < 4; i++) {
        pane->setVtxColor(i, TeamColors::Get(raceScenario.players[playerId].spTeam));
    }
}

s8 CtrlRaceBase::getPlayerId() const {
    auto *section = SectionManager::Instance()->currentSection();
    if (section->id() == SectionId::GhostReplay) {
        auto *racePage = getPage()->downcast<RacePage>();
        if (racePage == nullptr) {
            return -1;
        } else {
            return racePage->watchedPlayerId();
        }
    }

    return REPLACED(getPlayerId)();
}

void CtrlRaceBase::calcLabelVisibility(const char *pane) {
    bool visible = System::RaceConfig::Instance()->raceScenario().localPlayerCount <= 2;
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::HUDLabels>();
    if (setting == SP::ClientSettings::HUDLabels::Hide) {
        visible = false;
    }
    setPaneVisible(pane, visible);
}

} // namespace UI
