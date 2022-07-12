#include "RacePage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/ctrl/CtrlRaceInputDisplay.hh"
#include "game/ui/ctrl/CtrlRaceSpeed.hh"

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

RacePage *RacePage::Instance() {
    return s_instance;
}

u8 RacePage::getControlCount(u32 controls) const {
    u8 count = REPLACED(getControlCount)(controls);

    u32 localPlayerCount = System::RaceConfig::Instance()->raceScenario().localPlayerCount;
    localPlayerCount = std::max(localPlayerCount, static_cast<u32>(1));
    count += localPlayerCount; // CtrlRaceSpeed
    count += localPlayerCount; // CtrlRaceInputDisplay

    if (getNameBalloonCount() != 0) {
        count += 12 - getNameBalloonCount();
    }

    return count;
}

void RacePage::initControls(u32 controls) {
    REPLACED(initControls)(controls);

    u32 index = getControlCount(controls) - 1;
    u32 localPlayerCount = System::RaceConfig::Instance()->raceScenario().localPlayerCount;
    localPlayerCount = std::max(localPlayerCount, static_cast<u32>(1));

    for (u32 i = 0; i < localPlayerCount; i++) {
        auto *control = new CtrlRaceSpeed;
        insertChild(index--, control, 0);
        control->load(localPlayerCount, i);
    }
    for (u32 i = 0; i < localPlayerCount; i++) {
        auto *control = new CtrlRaceInputDisplay;
        insertChild(index--, control, 0);
        control->load(localPlayerCount, i);
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
