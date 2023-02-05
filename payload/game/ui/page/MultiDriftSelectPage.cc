#include "MultiDriftSelectPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

void MultiDriftSelectPage::onButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    // Back button processing
    s32 buttonIdx = button->m_index;
    if (buttonIdx == -100 && localPlayerId == 0) {
        return onBackButtonFront(button);
    }

    // Drift selection processing
    if (buttonIdx < 0 || buttonIdx > 7) {
        return;
    }

    assert(localPlayerId == static_cast<u32>(buttonIdx / 2));
    auto *sectionManager = SectionManager::Instance();
    u8 selection = buttonIdx % 2 == 0 ? 1 : 0;

    sectionManager->registeredPadManager().setDriftIsAuto(localPlayerId, selection);
    sectionManager->globalContext()->m_driftModes[localPlayerId] = selection + 1;

    // Hide non-selected button
    s8 inverse = selection == 0 ? -1 : 1;
    m_buttons[buttonIdx + inverse]->setVisible(false);

    // Lock controls
    getInputManager()->setPerControl(localPlayerId, false);

    // Try to exit page
    if (!checkAllMulti()) {
        return;
    }

    auto sectionId = sectionManager->currentSection()->id();

    if (Section::GetSceneId(sectionId) == 4 /* Globe */) {
        if (m_replacementSection == SectionId::None) {
            m_replacement = PageId::None;
            f32 delay = button->getDelay();
            Page::startReplace(Anim::Next, delay);
        } else {
            requestChangeSection(m_replacementSection, button);
        }
    } else {
        startReplace(PageId::CourseSelect, button);
    }
}

} // namespace UI
