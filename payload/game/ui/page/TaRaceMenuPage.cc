#include "TaRaceMenuPage.hh"

#include <features/save_states/SaveStates.hh>

namespace UI {

static const RaceMenuPage::ButtonId timeAttackPauseButtons[] = {
        RaceMenuPage::ButtonId::Continue1,
        RaceMenuPage::ButtonId::Restart1,
#if ENABLE_SAVE_STATES
        RaceMenuPage::ButtonId::SaveState,
        RaceMenuPage::ButtonId::LoadState,
#endif
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

const char *TaRaceMenuPage::getResFileName() {
#if ENABLE_SAVE_STATES
    return "PauseMenuTimeAttack";
#else
    return "PauseMenuTimeAttackNoSS";
#endif
}

} // namespace UI
