#include "ServicePackChannelPage.hh"

namespace UI {

ServicePackChannelPage::ServicePackChannelPage() = default;

ServicePackChannelPage::~ServicePackChannelPage() = default;

void ServicePackChannelPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(3);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_rankingsButton, 0);
    insertChild(2, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_rankingsButton.load("button", "ServicePackChannelButton", "Rankings", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_rankingsButton.setFrontHandler(&m_onRankingsButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(10428);

    m_rankingsButton.selectDefault(0);
}

void ServicePackChannelPage::onBack(u32 /* localPlayerId */) {
    changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
}

void ServicePackChannelPage::onRankingsButtonFront(PushButton *pushButton,
        u32 /* localPlayerId */) {
    changeSection(SectionId::ServicePackRankings, Anim::Next, pushButton->getDelay());
}

void ServicePackChannelPage::onBackButtonFront(PushButton *pushButton, u32 /* localPlayerId */) {
    changeSection(SectionId::TitleFromMenu, Anim::Prev, pushButton->getDelay());
}

} // namespace UI
