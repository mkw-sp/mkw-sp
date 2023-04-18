#include "GhostWatchRaceMenuPage.hh"

namespace UI {

static const RaceMenuPage::ButtonId ghostWatchPauseButtons[] = {
        RaceMenuPage::ButtonId::Continue2,
        RaceMenuPage::ButtonId::Restart3,
        RaceMenuPage::ButtonId::BattleGhost,
        RaceMenuPage::ButtonId::ChangeGhostData,
        RaceMenuPage::ButtonId::ChangeCourse,
        RaceMenuPage::ButtonId::ChangeCharacter,
        RaceMenuPage::ButtonId::Settings,
        RaceMenuPage::ButtonId::Quit1,
};

s32 GhostWatchRaceMenuPage::getButtonCount() {
    return std::size(ghostWatchPauseButtons);
}

const RaceMenuPage::ButtonId *GhostWatchRaceMenuPage::getButtons(void) {
    return ghostWatchPauseButtons;
}

bool GhostWatchRaceMenuPage::getOptionClosable() {
    // Allow closing with option
    return true;
}

} // namespace UI
