#include "NoLongerAwesomePage.hh"

#include "game/ui/GlobePage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

NoLongerAwesomePage::NoLongerAwesomePage() = default;

NoLongerAwesomePage::~NoLongerAwesomePage() = default;

void NoLongerAwesomePage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(4);
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        insertChild(i, &m_buttons[i], 0);
    }
    insertChild(3, &m_backButton, 0);

    m_buttons[0].load("button", "ServicePackTopButton", "Update", 0x1, false, false);
    m_buttons[1].load("button", "ServicePackTopButton", "Channel", 0x1, false, false);
    m_buttons[2].load("button", "ServicePackTopButton", "About", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        m_buttons[i].setFrontHandler(&m_onButtonFront, false);
    }
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (size_t i = 0; i < m_buttons.size(); ++i) {
        m_buttons[i].m_index = i;
    }

    m_buttons[0].selectDefault(0);
}

void NoLongerAwesomePage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *globePage = section->page<PageId::Globe>();
    if (!section->isPageFocused(this)) {
        section->pullPage();
    }
    switch (button->m_index) {
    case 0:
        push(PageId::NoLongerAwesomeSub1, Anim::None);
        return globePage->requestSpinClose();
    case 1:
        push(PageId::NoLongerAwesomeSub2, Anim::None);
        return globePage->requestSpinMid();
    case 2:
        push(PageId::NoLongerAwesomeSub3, Anim::None);
        return globePage->requestSpinFar();
    default:
        break;
    }
}

void NoLongerAwesomePage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    changeSection(SectionId::TitleFromMenu, Anim::Prev, button->getDelay());
}

void NoLongerAwesomePage::onBack(u32 /* localPlayerId */) {
    changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
}

} // namespace UI
