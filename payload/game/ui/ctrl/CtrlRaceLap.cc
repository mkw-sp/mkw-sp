#include "CtrlRaceLap.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

void CtrlRaceLap::calcSelf() {
    calcLabelVisibility("lap_text");

    REPLACED(calcSelf)();
}

void CtrlRaceLap::load(const char *variant, u32 localPlayerId) {
    m_localPlayerId = localPlayerId;

    const char *file = SectionManager::Instance()->taIsVanilla() ? "lap_number" : "lap_number_sp";
    const char *groups[] = {
        "eAFKartLap",
        "texture_pattern_0_9_0",
        nullptr,
        "eAFGoalLap",
        "texture_pattern_0_9_1",
        nullptr,
        nullptr,
    };
    LayoutUIControl::load("game_image", file, variant, groups);
}

} // namespace UI
