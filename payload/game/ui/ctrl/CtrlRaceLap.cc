#include "CtrlRaceLap.hh"

#include "game/host_system/SystemManager.hh"
#include "game/system/SaveManager.hh"

namespace UI {

namespace GroupId {

enum {
    Position = 2,
};

} // namespace GroupId

CtrlRaceLap::CtrlRaceLap() = default;

CtrlRaceLap::~CtrlRaceLap() = default;

void CtrlRaceLap::calcSelf() {
    calcLabelVisibility("lap_text");

    REPLACED(calcSelf)();

    u32 positionId = 14;
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::Speedometer>();
    if (setting != SP::ClientSettings::Speedometer::Off) {
        u32 aspectRatio = System::SystemManager::Instance()->aspectRatio();
        positionId = (m_screenCount + m_localPlayerId - 1) * 2 + aspectRatio;
    }
    m_animator.setAnimationInactive(GroupId::Position, 0, positionId);
}

void CtrlRaceLap::load(u32 localPlayerCount, u32 localPlayerId) {
    m_localPlayerId = localPlayerId;
    m_screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
    char variant[0x20];
    snprintf(variant, std::size(variant), "CtrlRaceLap_%u_%u", m_screenCount, localPlayerId);
    // clang-format off
    const char *groups[] = {
        "eAFKartLap", "texture_pattern_0_9_0", nullptr,
        "eAFGoalLap", "texture_pattern_0_9_1", nullptr,
        "Position", "position", nullptr,
        nullptr,
    };
    // clang-format on
    LayoutUIControl::load("game_image", "lap_number", variant, groups);
}

} // namespace UI
