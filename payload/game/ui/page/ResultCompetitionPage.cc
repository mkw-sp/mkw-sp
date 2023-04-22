#include "ResultCompetitionPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

PageId ResultCompetitionPage::getReplacement() {
    auto currentSectionId = SectionManager::Instance()->currentSection()->id();
    if (currentSectionId == SectionId::MR) {
        return PageId::AfterMrMenu;
    }

    return PageId::AfterToMenu;
}

} // namespace UI