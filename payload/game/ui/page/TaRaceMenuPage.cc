#include "TaRaceMenuPage.hh"

namespace UI {

static const RaceMenuPage::ButtonId timeAttackPauseButtons[] = {
        RaceMenuPage::ButtonId::Continue1,
        RaceMenuPage::ButtonId::Restart1,
        RaceMenuPage::ButtonId::SaveState,
        RaceMenuPage::ButtonId::LoadState,
        RaceMenuPage::ButtonId::ChangeGhostData,
        RaceMenuPage::ButtonId::ChangeCourse,
        RaceMenuPage::ButtonId::ChangeCharacter,
        RaceMenuPage::ButtonId::Settings,
        RaceMenuPage::ButtonId::Quit1,
};

s32 TaRaceMenuPage::getButtonCount() {
    return std::size(timeAttackPauseButtons);
}

const RaceMenuPage::ButtonId *TaRaceMenuPage::getButtons() {
    return timeAttackPauseButtons;
}

} // namespace UI
