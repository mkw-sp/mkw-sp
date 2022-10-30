#include "OnlineTeamSelectButton.hh"

#include "game/ui/TeamColors.hh"

namespace UI {

OnlineTeamSelectButton::OnlineTeamSelectButton() = default;

OnlineTeamSelectButton::~OnlineTeamSelectButton() = default;

void OnlineTeamSelectButton::refresh(u32 teamId) {
    const char *paneNames[2] = { "black_parts_c_r", "black_parts_c_l" };
    for (size_t i = 0; i < std::size(paneNames); i++) {
        auto *pane = m_mainLayout.findPaneByName(paneNames[i]);
        assert(pane);
        auto *material = pane->getMaterial();
        assert(material);
        auto color = TeamColors::Get(teamId);
        material->tevColors[0] = { color.r, color.g, color.b, color.a };
        material->tevColors[1] = { color.r, color.g, color.b, color.a };
    }
}

} // namespace UI
