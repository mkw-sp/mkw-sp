#include "ToRaceMenuPage.hh"
#if ENABLE_MR

#include "game/ui/SectionManager.hh"
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

static const RaceMenuPage::ButtonId TournamentPauseButtons[] = {
        RaceMenuPage::ButtonId::Continue1,
        RaceMenuPage::ButtonId::Restart1,
        RaceMenuPage::ButtonId::ChangeCharacter,
        RaceMenuPage::ButtonId::Quit1,
};

static const RaceMenuPage::ButtonId MissionRunPauseButtons[] = {
        RaceMenuPage::ButtonId::Continue1,
        RaceMenuPage::ButtonId::Restart1,
        RaceMenuPage::ButtonId::ChangeMission,
        RaceMenuPage::ButtonId::Quit1,
};

const RaceMenuPage::ButtonId *ToRaceMenuPage::getButtons() {
    auto currentSectionid = SectionManager::Instance()->currentSection()->id();
    if (currentSectionid == SectionId::MR) {
        return MissionRunPauseButtons;
    } else {
        return TournamentPauseButtons;
    }
}

const char *ToRaceMenuPage::getResFileName() {
    auto currentSectionid = SectionManager::Instance()->currentSection()->id();
    if (currentSectionid == SectionId::MR) {
        return "PauseMenuMR";
    } else {
        return "PauseMenuEvent";
    }
}

} // namespace UI

#endif
