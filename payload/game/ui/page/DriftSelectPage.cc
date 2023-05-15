#include "DriftSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/MissionInstructionPage.hh"

#include <sp/CourseDatabase.hh>
#include <sp/settings/ClientSettings.hh>

namespace UI {

void DriftSelectPage::onActivate() {
    REPLACED(onActivate)();

    auto *sectionManager = SectionManager::Instance();
    selectDefault(m_buttons[sectionManager->globalContext()->m_driftModes[0] != 1]);
}

void DriftSelectPage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *sectionManager = SectionManager::Instance();
    auto *section = sectionManager->currentSection();
    auto sectionId = section->id();

    switch (button->m_index) {
    case 0:
    case 1:
        sectionManager->registeredPadManager().setDriftIsAuto(0, button->m_index);
        sectionManager->globalContext()->m_driftModes[0] = button->m_index + 1;
        if (Section::GetSceneId(sectionId) == 4 /* Globe */) {
            if (m_replacementSection == SectionId::None) {
                m_replacement = PageId::None;
                f32 delay = button->getDelay();
                Page::startReplace(Anim::Next, delay);
            } else {
                requestChangeSection(m_replacementSection, button);
            }
        } else {
            auto *raceConfig = System::RaceConfig::Instance();
            auto &menuScenario = raceConfig->menuScenario();
            if (menuScenario.gameMode == System::RaceConfig::GameMode::Mission) {
                auto *missionInstructionPage = section->page<PageId::MissionInstruction>();
                if (missionInstructionPage->levelId() == 7 /* Boss */) {
                    menuScenario.cameraMode = 5;
                    requestChangeSection(SectionId::MRBossDemo, button);
                } else {
                    requestChangeSection(SectionId::MR, button);
                }
            } else if (raceConfig->selectRandomCourse()) {
                if (menuScenario.gameMode == System::RaceConfig::GameMode::OfflineBT) {
                    changeSection(SectionId::BTDemo, Anim::Next, 0.0f);
                } else {
                    changeSection(SectionId::VSDemo, Anim::Next, 0.0f);
                }
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
