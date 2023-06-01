#include "PackSelectPage.hh"

#include "game/ui/CourseSelectPage.hh"
#include "game/ui/ModelPage.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"

#include <game/system/RaceConfig.hh>
#include <sp/CourseDatabase.hh>

#include <cstdio>

namespace UI {

PackSelectPage::PackSelectPage() = default;

PackSelectPage::~PackSelectPage() = default;

PageId PackSelectPage::getReplacement() {
    return m_replacement;
}

void PackSelectPage::onInit() {
    auto *section = SectionManager::Instance()->currentSection();
    bool isOnline = section->isPageActive(PageId::OnlineConnectionManager);

    auto packCount = 1 - isOnline;

    m_sheetCount = (packCount + std::size(m_buttons) - 1) / std::size(m_buttons);
    m_sheetIndex = 0;
    m_lastSelected = 0;

    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(5 + std::size(m_buttons));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_sheetSelect, 0);
    insertChild(2, &m_sheetLabel, 0);
    insertChild(3, &m_scrollBar, 0);
    insertChild(4, &m_backButton, 0);
    for (u32 i = 0; i < std::size(m_buttons); i++) {
        insertChild(5 + i, &m_buttons[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < std::size(m_buttons); i++) {
        char variant[0x20];
        snprintf(variant, std::size(variant), "Button%zu", i);
        m_buttons[i].load("button", "PackSelectButton", variant, 0x1, false, false);
        m_buttons[i].m_index = i;
    }
    m_sheetSelect.load("button", "CourseSelectArrowRight", "ButtonArrowRight",
            "CourseSelectArrowLeft", "ButtonArrowLeft", 0x1, false, false);
    m_sheetLabel.load("control", "TimeAttackGhostListPageNum", "TimeAttackGhostListPageNum", NULL);
    m_scrollBar.load(m_sheetCount, m_sheetIndex, "button", "CourseSelectScrollBar",
            "CourseSelectScrollBar", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < m_buttons.size(); i++) {
        m_buttons[i].setFrontHandler(&m_onButtonFront, false);
        m_buttons[i].setSelectHandler(&m_onButtonSelect, false);
    }
    m_sheetSelect.setRightHandler(&m_onSheetSelectRight);
    m_sheetSelect.setLeftHandler(&m_onSheetSelectLeft);
    m_scrollBar.setChangeHandler(&m_onScrollBarChange);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(10394);

    m_sheetSelect.setVisible(m_sheetCount > 1);
    m_sheetSelect.setPlayerFlags(m_sheetCount <= 1 ? 0x0 : 0x1);

    m_buttons[0].selectDefault(0);

    refresh();
}

void PackSelectPage::onActivate() {
    m_replacement = PageId::None;

    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->modelControl().setModel(-1);
}

void PackSelectPage::onBack(u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    if (section->isPageActive(PageId::OnlineConnectionManager)) {
        m_replacement = PageId::OnlineTop;
        startReplace(Anim::Prev, 0.0f);
    } else {
        changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
    }
}

void PackSelectPage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *connManager = section->page<PageId::OnlineConnectionManager>();

    if (connManager != nullptr) {
        connManager->setTrackpack(button->m_index * m_sheetIndex);
        m_replacement = PageId::OnlineModeSelect;
    } else {
        auto buttonIndex = m_sheetIndex * m_buttons.size() + button->m_index;
        if (!s_lastPackFront.has_value() || *s_lastPackFront != buttonIndex) {
            SP::CourseDatabase::Instance().resetSelection();
        }

        m_replacement = PageId::SingleTop;
    }

    startReplace(Anim::Next, button->getDelay());
}

void PackSelectPage::onButtonSelect(PushButton *button, u32 /* localPlayerId */) {
    m_lastSelected = button->m_index;
}

void PackSelectPage::onSheetSelectRight(SheetSelectControl * /* control */,
        u32 /* localPlayerId */) {
    if (m_sheetIndex == m_sheetCount - 1) {
        m_sheetIndex = 0;
    } else {
        ++m_sheetIndex;
    }

    refresh();

    while (!m_buttons[m_lastSelected].getVisible()) {
        --m_lastSelected;
    }
    m_buttons[m_lastSelected].select(0);

    m_scrollBar.m_chosen = m_sheetIndex;
}

void PackSelectPage::onSheetSelectLeft(SheetSelectControl * /* control */,
        u32 /* localPlayerId */) {
    if (m_sheetIndex == 0) {
        m_sheetIndex = m_sheetCount - 1;
    } else {
        --m_sheetIndex;
    }

    refresh();

    while (!m_buttons[m_lastSelected].getVisible()) {
        --m_lastSelected;
    }
    m_buttons[m_lastSelected].select(0);

    m_scrollBar.m_chosen = m_sheetIndex;
}

void PackSelectPage::onScrollBarChange(ScrollBar * /* scrollBar */, u32 /* localPlayerId */,
        u32 chosen) {
    m_sheetIndex = chosen;

    refresh();
}

void PackSelectPage::onBackButtonFront(PushButton * /* button */, u32 localPlayerId) {
    onBack(localPlayerId);
}

void PackSelectPage::refresh() {
    auto *section = SectionManager::Instance()->currentSection();
    bool isOnline = section->isPageActive(PageId::OnlineConnectionManager);

    u32 packCount = 1 - isOnline;
    for (size_t i = isOnline; i < m_buttons.size(); i++) {
        u32 packIndex = m_sheetIndex * m_buttons.size() + i;
        if (packIndex < packCount) {
            m_buttons[i].setVisible(true);
            m_buttons[i].setPlayerFlags(1 << 0);

            MessageInfo info{};
            info.strings[0] = L"Nintendo Tracks";
            m_buttons[i].setMessageAll(6602, &info);
        } else {
            m_buttons[i].setVisible(false);
            m_buttons[i].setPlayerFlags(0);
        }
    }

    m_sheetLabel.setVisible(m_sheetCount != 0);
    MessageInfo info{};
    info.intVals[0] = m_sheetIndex + 1;
    info.intVals[1] = m_sheetCount;
    m_sheetLabel.setMessageAll(2009, &info);
}

std::optional<u32> PackSelectPage::s_lastPackFront = std::nullopt;

} // namespace UI
