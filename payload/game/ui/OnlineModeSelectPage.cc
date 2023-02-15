#include "OnlineModeSelectPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

// Replacement needed as stock game fetches the isRegional flag from
// the RandomMatching page to show the "Worldwide" or "Regional" text
// which we replaced and is therefore invalid.
void OnlineModeSelectPage::onActivate() {
    auto section = SectionManager::Instance()->currentSection();
    auto connectionManager = section->page<PageId::OnlineConnectionManager>();

    if (connectionManager->isCustomTrackpack()) {
        // TODO: Show the name of the pack selected?
        m_pageTitleText.setMessage(0xfa1);
    } else {
        m_pageTitleText.setMessage(0xfa0);
    }
}

void OnlineModeSelectPage::onButtonFront(PushButton *button, u32 localPlayerId) {
    auto section = SectionManager::Instance()->currentSection();
    auto connectionManager = section->page<PageId::OnlineConnectionManager>();

    if (button->m_index == 1) {
        connectionManager->setGamemode(0);
    } else if (button->m_index == 2) {
        connectionManager->setGamemode(1);
    }

    m_replacement = PageId::RandomMatching;
    startReplace(Anim::Next, button->getDelay());
}

}
