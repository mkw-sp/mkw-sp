#include "TitlePage.hh"

#include "game/ui/MessagePage.hh"
#include "game/ui/SectionManager.hh"

extern "C" {
#include <sp/Host.h>
}

#include <vendor/libhydrogen/hydrogen.h>

namespace UI {

void TitlePage::onInit() {
    REPLACED(onInit)();

    auto globalContext = SectionManager::Instance()->globalContext();

    m_useAlternativeBackgrounds = true;
    do {
        m_alternativeBackground = hydro_random_uniform(5);
    } while (m_alternativeBackground == globalContext->m_lastTitleBackground);
}

void TitlePage::onActivate() {
    REPLACED(onActivate)();
    if (m_geckoWarningShown || !Host_IsGeckoEnabled()) {
        return;
    }

    m_geckoWarningShown = true;

    auto section = SectionManager::Instance()->currentSection();
    auto *popupPage = section->page<PageId::MessagePopup>();

    popupPage->reset();
    popupPage->setWindowMessage(10055);
    push(PageId::MessagePopup, Anim::None);
}

} // namespace UI
