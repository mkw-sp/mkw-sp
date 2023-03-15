#include "AwardDemoResultItem.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/TeamColors.hh"

namespace UI {

namespace GroupId {

enum {
    Loop = 0,
    Player = 1,
    PlayerColorOn = 2,
    PlayerColorType = 3,
    CPUColorType = 4,
    PlayerFlash = 5,
};

} // namespace GroupId

AwardDemoResultItem::AwardDemoResultItem() = default;

AwardDemoResultItem::~AwardDemoResultItem() = default;

void AwardDemoResultItem::refresh(u32 playerId, bool isMultiPlayer, u32 soloRank) {
    refresh(playerId, false, isMultiPlayer);

    setMessage("position", 1300 + soloRank);

    const auto &awardsScenario = System::RaceConfig::Instance()->awardsScenario();
    nw4r::lyt::Pane *pane;
    if (awardsScenario.players[playerId].type == System::RaceConfig::Player::Type::Local) {
        m_animator.setAnimation(GroupId::PlayerColorType, 4, 0.0f);
        pane = m_mainLayout.findPaneByName("p_color_r");
    } else {
        m_animator.setAnimation(GroupId::CPUColorType, 1, 0.0f);
        pane = m_mainLayout.findPaneByName("team_color_c");
    }
    assert(pane);
    pane->m_visible = true;
    auto *material = pane->getMaterial();
    assert(material);
    auto color = TeamColors::Get(awardsScenario.players[playerId].spTeam);
    material->tevColors[0] = {color.r, color.g, color.b, color.a};
    material->tevColors[1] = {color.r, color.g, color.b, color.a};
}

} // namespace UI
