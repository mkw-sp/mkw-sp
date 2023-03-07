#include "CtrlRaceRankNum.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/Page.hh"

namespace UI {

void CtrlRaceRankNum::init() {
    m_animator.setAnimationInactive(0, 0, m_rank - 1.0f);
    m_animator.setAnimation(1, 1, 0.0f);
    m_playerId = getPlayerId();
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.localPlayerCount < 2 && raceScenario.spMaxTeamSize < 2) {
        m_animator.setAnimationInactive(2, 0, 0.0f);
    } else {
        setPaneColor("position", true);
    }
    _1a0 = m_mainLayout.findPaneByName("position_l_00");
    LayoutUIControl::init();
}

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

void CtrlRaceRankNum::load(const char *variant, u32 localPlayerId) {
    m_localPlayerId = localPlayerId;
    u32 playerId = getPlayerId();
    m_rank = System::RaceManager::Instance()->player(playerId)->rank();
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    // clang-format off
    if (raceScenario.localPlayerCount < 2 && raceScenario.spMaxTeamSize < 2) {
        const char *groups[] = {
            "eRank", "texure_patten_rank", nullptr,
            "eRankTransition", "eClose", "eOpen", "eWait", nullptr,
            "eColor", "default", "blue", "red", nullptr,
            nullptr,
        };
        LayoutUIControl::load("game_image", "position", variant, groups);
    } else {
        const char *groups[] = {
            "eRank", "texure_patten_rank", nullptr,
            "eRankTransition", "eClose", "eOpen", "eWait", nullptr,
            nullptr,
        };
        LayoutUIControl::load("game_image", "position_multi", variant, groups);
    }
    // clang-format on
}

bool CtrlRaceRankNum::isDisabled() const {
    auto pageId = getPage()->id();
    if (pageId != PageId::TaHud && pageId != PageId::GhostReplayHud) {
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
