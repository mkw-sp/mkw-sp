#include "TimeAttackSplitsPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

TimeAttackSplitsPage *TimeAttackSplitsPage::ct() {
    REPLACED(ct)();
    m_isReady = false;
    return this;
};

PageId TimeAttackSplitsPage::getReplacement() {
    return PageId::AfterTaMenu;
}

void TimeAttackSplitsPage::afterCalc() {
    if (state() != State::State4) {
        return;
    }

    if (m_isReady) {
        return;
    }

    auto *sectionManager = SectionManager::Instance();
    if (sectionManager->saveManagerProxy()->hasPendingRequests()) {
        return;
    }

    auto *saveManager = System::SaveManager::Instance();
    if (saveManager->saveGhostResult()) {
        m_ghostMessage.setMessageAll(1115);
    } else {
        if (saveManager->m_usedItemWheel) {
            m_ghostMessage.setMessageAll(10452);
        } else {
            m_ghostMessage.setMessageAll(1116);
        }
    }

    m_isReady = true;
};

void TimeAttackSplitsPage::onFront() {
    if (!m_isReady) {
        return;
    }

    startReplace(Anim::Prev, 0.0f);
}

} // namespace UI
