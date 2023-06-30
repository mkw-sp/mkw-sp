/// Notes:
/// - The first pack displayed is the "Nintendo Tracks" pack, which contains vanilla courses.
/// This is hidden when online, due to the OnlineTopPage already allowing the user to pick
/// "Nintendo Tracks" vs "Track Packs".

#include "PackSelectPage.hh"

#include "game/ui/CourseSelectPage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/ModelPage.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"

#include <sp/trackPacks/TrackPackManager.hh>

#include <cstdio>

namespace UI {

PackSelectPage::PackSelectPage() = default;

PackSelectPage::~PackSelectPage() = default;

PageId PackSelectPage::getReplacement() {
    return m_replacement;
}

void PackSelectPage::onInit() {
    SP::TrackPackManager::CreateInstance();
    auto &trackPackManager = SP::TrackPackManager::Instance();

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    auto packCount = trackPackManager.getPackCount() - Section::HasOnlineManager(sectionId);

    m_sheetCount = (packCount + m_buttons.size() - 1) / m_buttons.size();
    m_sheetIndex = 0;
    m_lastSelected = 0;

    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(5 + m_buttons.size());
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_sheetSelect, 0);
    insertChild(2, &m_sheetLabel, 0);
    insertChild(3, &m_scrollBar, 0);
    insertChild(4, &m_backButton, 0);
    for (size_t i = 0; i < m_buttons.size(); i++) {
        insertChild(5 + i, &m_buttons[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < m_buttons.size(); i++) {
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

void PackSelectPage::onDeinit() {
    SP::TrackPackManager::DestroyInstance();
}

void PackSelectPage::onActivate() {
    m_replacement = PageId::None;
    m_scrollBar.reconfigure(m_sheetCount, m_sheetIndex, m_sheetCount >= 2 ? 0x1 : 0x0);

    auto *section = SectionManager::Instance()->currentSection();
    auto *connManager = section->page<PageId::OnlineConnectionManager>();
    if (connManager == nullptr) {
        auto *modelPage = section->page<PageId::Model>();
        modelPage->modelControl().setModel(-1);
    }
}

void PackSelectPage::onBack(u32 /* localPlayerId */) {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (Section::HasOnlineManager(sectionId)) {
        m_replacement = PageId::OnlineTop;
    } else if (sectionId == SectionId::Multi) {
        m_replacement = PageId::PlayerPad;
    } else {
        changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
        return;
    }

    startReplace(Anim::Prev, 0.0f);
}

void PackSelectPage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *sectionManager = SectionManager::Instance();
    auto *globalContext = sectionManager->globalContext();
    auto sectionId = sectionManager->currentSection()->id();

    bool isOnline = Section::HasOnlineManager(sectionId);

    auto buttonIndex = m_sheetIndex * m_buttons.size() + button->m_index + isOnline;
    globalContext->m_currentPack = buttonIndex;

    if (isOnline) {
        m_replacement = PageId::OnlineModeSelect;
    } else {
        if (!s_lastPackFront.has_value() || *s_lastPackFront != buttonIndex) {
            UI::CourseSelectPage::s_lastSelected.reset();
        }

        s_lastPackFront = buttonIndex;
        if (sectionId == SectionId::Multi) {
            m_replacement = PageId::MultiTop;
        } else {
            m_replacement = PageId::SingleTop;
        }
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
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    bool isOnline = Section::HasOnlineManager(sectionId);

    auto &trackPackManager = SP::TrackPackManager::Instance();
    u32 packCount = trackPackManager.getPackCount();
    for (size_t i = 0; i < m_buttons.size(); i++) {
        u32 packIndex = m_sheetIndex * m_buttons.size() + i + isOnline;
        if (packIndex < packCount) {
            m_buttons[i].setVisible(true);
            m_buttons[i].setPlayerFlags(1 << 0);

            MessageInfo info{};
            info.strings[0] = trackPackManager.getNthPack(packIndex).getPrettyName();
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
