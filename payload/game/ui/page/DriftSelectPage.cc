#include "DriftSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/MissionInstructionPage.hh"

namespace UI {

void DriftSelectPage::onActivate() {
    REPLACED(onActivate)();

    auto *sectionManager = SectionManager::Instance();
    selectDefault(m_buttons[sectionManager->globalContext()->m_driftModes[0] != 1]);
}

void DriftSelectPage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *sectionManager = SectionManager::Instance();
    auto *globalContext = sectionManager->globalContext();

    auto *section = sectionManager->currentSection();
    auto sectionId = section->id();

    switch (button->m_index) {
    case 0:
    case 1:
        sectionManager->registeredPadManager().setDriftIsAuto(0, button->m_index);
        globalContext->m_driftModes[0] = button->m_index + 1;
        if (Section::GetSceneId(sectionId) == System::SceneId::Globe) {
            if (m_replacementSection == SectionId::None) {
                m_replacement = PageId::None;
                f32 delay = button->getDelay();
                Page::startReplace(Anim::Next, delay);
            } else {
                requestChangeSection(m_replacementSection, button);
            }
        } else {
            auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
            if (menuScenario.gameMode == System::RaceConfig::GameMode::Mission) {
                auto *missionInstructionPage = section->page<PageId::MissionInstruction>();
                if (missionInstructionPage->levelId() == 7 /* Boss */) {
                    menuScenario.cameraMode = 5;
                    requestChangeSection(SectionId::MRBossDemo, button);
                } else {
                    requestChangeSection(SectionId::MR, button);
                }
            } else if (globalContext->generateRandomCourses()) {
                if (menuScenario.gameMode == System::RaceConfig::GameMode::OfflineBT) {
                    changeSection(SectionId::BTDemo, Anim::Next, 0.0f);
                } else {
                    changeSection(SectionId::VSDemo, Anim::Next, 0.0f);
                }
            } else if (sectionId == SectionId::SingleChannelLeaderboardChallenge ||
                    sectionId == SectionId::SingleGhostListChallenge) {
                requestChangeSection(SectionId::TA, button);
            } else {
                startReplace(PageId::CourseSelect, button);
            }
        }
        break;
    case 2:
        pushMessage(3309);
        button->setFrontSoundId(Sound::SoundId::SE_DUMMY);
        break;
    case -100:
        onBackButtonFront(button);
        break;
    default:
        break;
    }
}

} // namespace UI
