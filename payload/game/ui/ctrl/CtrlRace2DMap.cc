#include "CtrlRace2DMap.hh"

#include "game/kart/KartObjectManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/ui/TeamColors.hh"
#include "game/system/SaveManager.hh"

#include <sp/settings/ClientSettings.hh>

namespace UI {

void CtrlRace2DMap::calcSelf() {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::MiniMap>();
	setVisible(setting == SP::ClientSettings::MiniMap::Disable);
    REPLACED(calcSelf)();
}
	
void CtrlRace2DMapCharacter::load(u32 playerId) {
    m_playerId = playerId;
    m_object = Kart::KartObjectManager::Instance()->object(playerId);
    const char *groups[] = {
        "rotate",
        "default",
        "rotate",
        nullptr,
        "shadow",
        "dummy",
        nullptr,
        nullptr,
    };
    LayoutUIControl::load("game_image", "map_chara", "map_chara", groups);
}

void CtrlRace2DMapCharacter::calcTransform(Vec3 *pos, void *r5, void *r6) {
    REPLACED(calcTransform)(pos, r5, r6);

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.spMaxTeamSize < 2) {
        return;
    }

    nw4r::lyt::Pane *panes[2] = { m_charaShadow0Pane, m_charaShadow1Pane };
    for (size_t i = 0; i < std::size(panes); i++) {
        panes[i]->m_visible = true;
        auto *material = panes[i]->getMaterial();
        assert(material);
        auto color = TeamColors::Get(raceScenario.players[m_playerId].spTeam);
        material->tevColors[0] = { color.r, color.g, color.b, color.a };
        material->tevColors[1] = { color.r, color.g, color.b, color.a };
    }
}

} // namespace UI
