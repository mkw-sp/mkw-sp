#include "ResultPlayerPage.hh"

#include "game/system/RaceConfig.hh"

namespace UI {

PageId ResultRaceUpdatePage::getReplacement() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    return raceScenario.spMaxTeamSize < 2 ? PageId::ResultRaceTotal : PageId::ResultTeamVSTotal;
}

} // namespace UI
