#include "ReplayGhostRaceMenuPage.hh"

namespace UI {

static const RaceMenuPage::ButtonId replayTAPauseButtons[] = {
        RaceMenuPage::ButtonId::ContinueReplay,
        RaceMenuPage::ButtonId::RestartReplay,
        RaceMenuPage::ButtonId::Settings,
        RaceMenuPage::ButtonId::QuitReplay,
};

s32 ReplayGhostRaceMenuPage::getButtonCount() {
    return std::size(replayTAPauseButtons);
}

const RaceMenuPage::ButtonId *ReplayGhostRaceMenuPage::getButtons() {
    return replayTAPauseButtons;
}

} // namespace UI
