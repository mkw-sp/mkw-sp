#include "RacePage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ctrl/CtrlRaceBattleAddPoint.hh"
#include "game/ui/ctrl/CtrlRaceBattlePoint.hh"
#include "game/ui/ctrl/CtrlRaceBattleTotalPoint.hh"
#include "game/ui/ctrl/CtrlRaceDebugPanel.hh"
#include "game/ui/ctrl/CtrlRaceInputDisplay.hh"
#include "game/ui/ctrl/CtrlRaceLap.hh"
#include "game/ui/ctrl/CtrlRaceSpeed.hh"
#include "game/ui/ctrl/CtrlRaceWaitSymbol.hh"

#include <sp/cs/RaceManager.hh>

namespace UI {

namespace Control {

enum {
    Time = 1 << 1,
    Count = 1 << 2,
    Map = 1 << 3,
    RankNum = 1 << 4,
    Lap = 1 << 5,
    ItemWindow = 1 << 6,
    Point = 1 << 7,
    GhostTimeDiff = 1 << 8,
    Live = 1 << 9,
    Score = 1 << 10,
    ItemBalloon = 1 << 11,
    GhostMessage = 1 << 12,
};

} // namespace Control

u32 RacePage::watchedPlayerId() const {
    return m_watchedPlayerId;
}

u32 RacePage::lastWatchedPlayerId() const {
    return m_lastWatchedPlayerId;
}

RacePage *RacePage::Instance() {
    return s_instance;
}

PageId RacePage::getReplacement() {
    switch (id()) {
    case PageId::Bt1pHud:
    case PageId::Bt2pHud:
    case PageId::Bt3pHud:
    case PageId::Bt4pHud:
        return PageId::ResultRaceUpdate;
    default:
        return m_replacement;
    }
}

void RacePage::onInit() {
    REPLACED(onInit)();

    m_lastWatchedPlayerId = m_watchedPlayerId;
}

void RacePage::afterCalc() {
    m_lastWatchedPlayerId = m_watchedPlayerId;

    REPLACED(afterCalc)();
}

u8 RacePage::getControlCount(u32 controls) const {
    u8 count = REPLACED(getControlCount)(controls);

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    u32 localPlayerCount = std::max(raceScenario.localPlayerCount, static_cast<u8>(1));

    if (controls & Control::Point) {
        if (raceScenario.spMaxTeamSize < 2) {
            count--; // CtrlRaceBattleTotalPoint
        }
    }

    if (localPlayerCount <= 2 || !(controls & Control::Point)) {
        count += localPlayerCount; // CtrlRaceSpeed
    }

    auto *saveManager = System::SaveManager::Instance();
    auto vanillaMode = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
    if (vanillaMode == SP::ClientSettings::VanillaMode::Disable) {
        count += localPlayerCount;     // CtrlRaceInputDisplay
        count += localPlayerCount < 2; // CtrlRaceDebugPanel
    }

    if (getNameBalloonCount() != 0) {
        count += 12 - getNameBalloonCount();
    }

    // Extend vanilla online logic to SP
    if (SP::RaceManager::Instance()) {
        count += (localPlayerCount * 2) + 1;
    }

    return count;
}

void RacePage::initControls(u32 controls) {
    REPLACED(initControls)(controls & ~(Control::Point | Control::Lap));

    u32 index = getControlCount(controls) - 1;
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    u32 localPlayerCount = std::max(raceScenario.localPlayerCount, static_cast<u8>(1));

    if (controls & Control::Lap) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceLap;
            insertChild(index--, control, 0);
            u32 screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
            control->load(screenCount, i);
        }
    }

    if (controls & Control::Point) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceBattlePoint;
            insertChild(index--, control, 0);
            u32 screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
            char variant[0x20];
            snprintf(variant, std::size(variant), "CtrlRacePoint_%u_%u", screenCount, i);
            control->load(variant, i);
        }
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceBattleAddPoint;
            insertChild(index--, control, 0);
            u32 screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
            char variant[0x20];
            snprintf(variant, std::size(variant), "BattleAddPoint_%u_%u", screenCount, i);
            control->load(variant, i);
        }
        if (raceScenario.spMaxTeamSize >= 2) {
            auto *control = new CtrlRaceBattleTotalPoint;
            insertChild(index--, control, 0);
            control->load();
        }
    }

    if (localPlayerCount <= 2 || !(controls & Control::Point)) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceSpeed;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
    }

    auto *saveManager = System::SaveManager::Instance();
    auto vanillaMode = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
    if (vanillaMode != SP::ClientSettings::VanillaMode::Enable) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceInputDisplay;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
        if (localPlayerCount < 2) {
            auto *control = new CtrlRaceDebugPanel;
            insertChild(index--, control, 0);
            control->load();
        }
    }

    if (SP::RaceManager::Instance()) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceWifiStartMessage;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceWifiFinishMessage;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
        auto *control = new CtrlRaceWaitSymbol;
        insertChild(index--, control, 0);
        control->load(1, 0);
    }
}

u32 TARacePage::getControls() const {
    u32 controls = 0;
    controls |= Control::Time;
    controls |= Control::Count;
    controls |= Control::Map;
    controls |= Control::RankNum;
    controls |= Control::Lap;
    controls |= Control::ItemWindow;
    controls |= Control::GhostTimeDiff;
    controls |= Control::GhostMessage;
    return controls;
}

u32 VSMultiRacePage::getControls() const {
    u32 controls = 0;
    controls |= Control::Time;
    controls |= Control::Count;
    controls |= Control::Map;
    controls |= Control::RankNum;
    controls |= Control::Lap;
    controls |= Control::ItemWindow;
    controls |= Control::ItemBalloon;
    return controls;
}

u32 GhostReplayRacePage::getControls() const {
    u32 controls = 0;
    controls |= Control::Time;
    controls |= Control::Count;
    controls |= Control::Map;
    controls |= Control::RankNum;
    controls |= Control::Lap;
    controls |= Control::ItemWindow;
    return controls;
}

} // namespace UI
