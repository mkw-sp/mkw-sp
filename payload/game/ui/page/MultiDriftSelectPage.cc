#include "MultiDriftSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

void MultiDriftSelectPage::onButtonFront(PushButton *button, u32 localPlayerId) {
    // Back button processing
    s32 buttonIdx = button->m_index;
    if (buttonIdx == -100 && localPlayerId == 0) {
        return onBackButtonFront(button);
    }

    // Drift selection processing
    if (buttonIdx < 0 || buttonIdx > 7) {
        return;
    }

    auto *sectionManager = SectionManager::Instance();
    auto *globalContext = sectionManager->globalContext();

    assert(localPlayerId == static_cast<u32>(buttonIdx / 2));
    u8 selection = buttonIdx % 2 == 0 ? 1 : 0;

    sectionManager->registeredPadManager().setDriftIsAuto(localPlayerId, selection);
    globalContext->m_driftModes[localPlayerId] = selection + 1;

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
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    if (Section::GetSceneId(sectionId) == System::SceneId::Globe) {
        if (m_replacementSection == SectionId::None) {
            m_replacement = PageId::None;
            f32 delay = button->getDelay();
            Page::startReplace(Anim::Next, delay);
        } else {
            requestChangeSection(m_replacementSection, button);
        }
    } else if (globalContext->generateRandomCourses()) {
        if (menuScenario.gameMode == System::RaceConfig::GameMode::OfflineBT) {
            changeSection(SectionId::BTDemo, Anim::Next, 0.0f);
        } else {
            changeSection(SectionId::VSDemo, Anim::Next, 0.0f);
        }
    } else {
        startReplace(PageId::CourseSelect, button);
    }
}

} // namespace UI
