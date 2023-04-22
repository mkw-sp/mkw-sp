#include "BtRaceMenuPage.hh"

namespace UI {

static const RaceMenuPage::ButtonId battlePauseButtons[] = {
        RaceMenuPage::ButtonId::Continue1,
        RaceMenuPage::ButtonId::Restart1,
        RaceMenuPage::ButtonId::Settings,
        RaceMenuPage::ButtonId::Quit1,
};

s32 BtRaceMenuPage::getButtonCount(void) {
    return std::size(battlePauseButtons);
}

const RaceMenuPage::ButtonId *BtRaceMenuPage::getButtons(void) {
    return battlePauseButtons;
}

const char *BtRaceMenuPage::getResFileName(void) {
    return "PauseMenuVS";
}

} // namespace UI
