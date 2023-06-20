#include "PlayerPadPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

PageId PlayerPadPage::getReplacement() {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (sectionId == SectionId::OnlineMultiConfigure) {
        return PageId::MultiTop;
    } else {
        return PageId::PackSelect;
    }
}

} // namespace UI