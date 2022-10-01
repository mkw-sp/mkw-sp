#include "TeamBoxControl.hh"

#include "game/ui/TeamColors.hh"

namespace UI {

namespace GroupId {

enum {
    Position = 0,
};

} // namespace GroupId

TeamBoxControl::TeamBoxControl() = default;

TeamBoxControl::~TeamBoxControl() = default;

void TeamBoxControl::load() {
    const char *groups[] = {
        "Position",
        "Position",
        nullptr,
        nullptr,
    };
    LayoutUIControl::load("button", "TeamBox", "TeamBox", groups);
}

void TeamBoxControl::refresh(u32 teamId, u32 positionId) {
    m_animator.setAnimationInactive(GroupId::Position, 0, positionId);
    auto *pane = m_mainLayout.findPaneByName("Window_00");
    assert(pane);
    auto *material = pane->getMaterial(1);
    assert(material);
    auto color = TeamColors::Get(teamId);
    material->tevColors[1] = { color.r, color.g, color.b, color.a };
}

} // namespace UI
