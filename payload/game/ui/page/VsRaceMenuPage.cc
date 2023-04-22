#include "VsRaceMenuPage.hh"

namespace UI {

static const RaceMenuPage::ButtonId vsPauseButtons[] = {
        RaceMenuPage::ButtonId::Continue1,
        RaceMenuPage::ButtonId::Restart1,
        RaceMenuPage::ButtonId::Settings,
        RaceMenuPage::ButtonId::Quit1,
};

s32 VsRaceMenuPage::getButtonCount() {
    return std::size(vsPauseButtons);
}

const RaceMenuPage::ButtonId *VsRaceMenuPage::getButtons() {
    return vsPauseButtons;
}

const char *VsRaceMenuPage::getResFileName() {
    return "PauseMenuVS";
}

} // namespace UI