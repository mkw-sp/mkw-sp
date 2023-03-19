#include "Page.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/page/OnlineFriendRegisterPage.hh"

namespace UI {

Page::~Page() = default;

void Page::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~Page();
    }
}

// HACK(GnomedDev): This is absolutely horrid, but avoids
// having to either patch the vtable in a worse way or
// entirely reimplementing the page.
PageId Page::getReplacement() {
    if (m_id == PageId::EnterFriendCode) {
        auto *page = reinterpret_cast<OnlineFriendRegisterPage *>(this);
        return page->replacedGetReplacement();
    } else {
        return REPLACED(getReplacement)();
    }
}

Page::State Page::state() const {
    return m_state;
}

Page::Anim Page::anim() const {
    return m_anim;
}

const MenuInputManager *Page::inputManager() const {
    return m_inputManager;
}

MenuInputManager *Page::inputManager() {
    return m_inputManager;
}

void Page::calc() {
    switch (m_state) {
    case State::State3:
    case State::State4:
    case State::State5:
        beforeCalc();
        m_controlGroup.calc();
        afterCalc();
        calcAnim();
        break;
    default:
        break;
    }

    // Override animation for instant transition
    switch (m_state) {
    case State::State3:
    case State::State5: {
        auto *saveManager = System::SaveManager::Instance();
        if (saveManager->getSetting<SP::ClientSettings::Setting::PageTransitions>() ==
                SP::ClientSettings::PageTransitions::Disable) {
            m_canProceed = true;
        }
        break;
    }
    default:
        break;
    }
}

} // namespace UI
