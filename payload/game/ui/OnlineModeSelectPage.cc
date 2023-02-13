#include "OnlineModeSelectPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

// Replacement needed as stock game fetches the isRegional flag from
// the RandomMatching page to show the "Worldwide" or "Regional" text
// which we replaced and is therefore invalid.
void OnlineModeSelectPage::onActivate() {
    auto section = SectionManager::Instance()->currentSection();
    auto matchingPage = section->page<PageId::RandomMatching>();

    if (matchingPage->m_trackpack == 0) {
        m_pageTitleText.setMessage(0xfa0);
    } else {
        // TODO: Show the name of the pack selected?
        m_pageTitleText.setMessage(0xfa1);
    }
}

}