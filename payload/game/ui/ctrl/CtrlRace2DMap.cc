#include "CtrlRace2DMap.hh"

#include "game/kart/KartObjectManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/TeamColors.hh"
#include "game/ui/page/RacePage.hh"

#include <sp/settings/ClientSettings.hh>

namespace UI {

void CtrlRace2DMap::calcSelf() {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::MiniMap>();
    setVisible(setting == SP::ClientSettings::MiniMap::Enable);
    REPLACED(calcSelf)();
}

void CtrlRace2DMapCharacter::load(u32 playerId) {
    // clang-format off
    const char *groups[] = {
        "rotate", "default", "rotate", nullptr,
        "shadow", "dummy", nullptr,
        nullptr,
    };
    // clang-format on

    m_playerId = playerId;
    m_object = Kart::KartObjectManager::Instance()->object(playerId);
    LayoutUIControl::load("game_image", "map_chara", "map_chara", groups);
}

void CtrlRace2DMapCharacter::calcTransform(Vec3 *pos, void *r5, void *r6) {
    REPLACED(calcTransform)(pos, r5, r6);

    auto *scorePane = m_mainLayout.findPaneByName("score");
    assert(scorePane);
    scorePane->m_trans.x = m_charaPane->m_trans.x + 10.0f * m_charaPane->m_scale.x;
    scorePane->m_trans.y = m_charaPane->m_trans.y - 10.0f * m_charaPane->m_scale.y;
    scorePane->m_alpha = m_charaPane->m_alpha;

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();

    u16 battleScore = System::RaceManager::Instance()->player(m_playerId)->battleScore();
    if (battleScore == 0) {
        clearMessage("score");
    } else {
        MessageInfo info{};
        info.intVals[0] = battleScore;
        u32 messageId = 10398;
        if (raceScenario.spMaxTeamSize >= 2) {
            messageId = 10393 + raceScenario.players[m_playerId].spTeam;
        } else if (raceScenario.players[m_playerId].type ==
                System::RaceConfig::Player::Type::Local) {
            messageId = 10406 + raceScenario.players[m_playerId].screenId;
        } else if (m_playerId == RacePage::Instance()->watchedPlayerId()) {
            messageId = 10406;
        }
        setMessage("score", messageId, &info);
    }

    if (raceScenario.spMaxTeamSize < 2) {
        return;
    }

    nw4r::lyt::Pane *panes[2] = {m_charaShadow0Pane, m_charaShadow1Pane};
    for (size_t i = 0; i < std::size(panes); i++) {
        panes[i]->m_visible = true;
        auto *material = panes[i]->getMaterial();
        assert(material);
        auto color = TeamColors::Get(raceScenario.players[m_playerId].spTeam);
        material->tevColors[0] = {color.r, color.g, color.b, color.a};
        material->tevColors[1] = {color.r, color.g, color.b, color.a};
    }
}

} // namespace UI
