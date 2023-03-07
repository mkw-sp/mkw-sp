#include "CtrlRaceLap.hh"

#include "game/system/SaveManager.hh"

namespace UI {

void CtrlRaceLap::calcSelf() {
    calcLabelVisibility("lap_text");

    REPLACED(calcSelf)();
}

void CtrlRaceLap::load(const char *variant, u32 localPlayerId) {
    m_localPlayerId = localPlayerId;

    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::Speedometer>();
    bool isLowered = setting == SP::ClientSettings::Speedometer::Off;

    const char *file = isLowered ? "lap_number" : "lap_number_sp";
    // clang-format off
    const char *groups[] = {
        "eAFKartLap", "texture_pattern_0_9_0", nullptr,
        "eAFGoalLap", "texture_pattern_0_9_1", nullptr,
        nullptr,
    };
    // clang-format on
    LayoutUIControl::load("game_image", file, variant, groups);
}

} // namespace UI
