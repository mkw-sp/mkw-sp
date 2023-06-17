#include "NoLongerAwesomeSubPage.hh"

namespace UI {

NoLongerAwesomeSubPage1::NoLongerAwesomeSubPage1() = default;

NoLongerAwesomeSubPage1::~NoLongerAwesomeSubPage1() = default;

void NoLongerAwesomeSubPage1::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(0);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false);
}

PageId NoLongerAwesomeSubPage1::getReplacement() {
    return PageId::None;
}

void NoLongerAwesomeSubPage1::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::None, 0.0f);
}

NoLongerAwesomeSubPage2::NoLongerAwesomeSubPage2() = default;

NoLongerAwesomeSubPage2::~NoLongerAwesomeSubPage2() = default;

void NoLongerAwesomeSubPage2::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(0);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false);
}

PageId NoLongerAwesomeSubPage2::getReplacement() {
    return PageId::None;
}

void NoLongerAwesomeSubPage2::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::None, 0.0f);
}

NoLongerAwesomeSubPage3::NoLongerAwesomeSubPage3() = default;

NoLongerAwesomeSubPage3::~NoLongerAwesomeSubPage3() = default;

void NoLongerAwesomeSubPage3::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(0);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false);
}

PageId NoLongerAwesomeSubPage3::getReplacement() {
    return PageId::None;
}

void NoLongerAwesomeSubPage3::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::None, 0.0f);
}

} // namespace UI
