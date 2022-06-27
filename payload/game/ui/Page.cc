#include "Page.hh"

extern "C" {
#include "game/system/SaveManager.h"
}

namespace UI {

Page::~Page() = default;

void Page::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~Page();
    }
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
    case State::State5:
        if (!SaveManager_getSetting(s_saveManager, kSetting_PageTransitions)) {
            m_canProceed = true;
        }
        break;
    default:
        break;
    }
}

} // namespace UI
