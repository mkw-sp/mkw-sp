#include "ResultPlayerPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

PageId ResultRaceUpdatePage::getReplacement() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    return raceScenario.spMaxTeamSize < 2 ? PageId::ResultRaceTotal : PageId::ResultTeamVSTotal;
}

PageId ResultRaceTotalPage::getReplacement() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.isBattle()) {
        if (RaceMenuPage::IsLastMatch()) {
            return PageId::AfterBtFinal;
        }
        return PageId::AfterBtMenu;
    }
    return REPLACED(getReplacement)();
}

} // namespace UI
