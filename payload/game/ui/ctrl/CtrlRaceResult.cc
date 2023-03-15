#include "CtrlRaceResult.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/TeamColors.hh"

namespace UI {

void CtrlRaceResult::refreshRankIconsColor(u32 rank, u32 playerId) {
    REPLACED(refreshRankIconsColor)(rank, playerId);

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.spMaxTeamSize < 2) {
        return;
    }

    nw4r::lyt::Pane *pane;
    if (raceScenario.players[playerId].type == System::RaceConfig::Player::Type::Local) {
        m_animator.setAnimation(4, 4, 0.0f);
        pane = m_mainLayout.findPaneByName("select_base");
        assert(pane);
    } else {
        pane = m_mainLayout.findPaneByName("team_color_c");
        assert(pane);
        pane->m_alpha = 150;
    }
    pane->m_visible = true;
    auto *material = pane->getMaterial();
    assert(material);
    auto color = TeamColors::Get(raceScenario.players[playerId].spTeam);
    material->tevColors[0] = {color.r, color.g, color.b, color.a};
    material->tevColors[1] = {color.r, color.g, color.b, color.a};
}

} // namespace UI
