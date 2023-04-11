#include "AfterTaRaceMenuPage.hh"

namespace UI {

static const RaceMenuPage::ButtonId afterTaButtons[] = {
        RaceMenuPage::ButtonId::Restart2,
        RaceMenuPage::ButtonId::ChangeGhostData,
        RaceMenuPage::ButtonId::ChangeCourse,
        RaceMenuPage::ButtonId::ChangeCharacter,
        RaceMenuPage::ButtonId::Replay,
        RaceMenuPage::ButtonId::Quit1,
};

s32 AfterTaRaceMenuPage::getButtonCount(void) {
    return std::size(afterTaButtons);
}

const RaceMenuPage::ButtonId *AfterTaRaceMenuPage::getButtons(void) {
    return afterTaButtons;
}

} // namespace UI