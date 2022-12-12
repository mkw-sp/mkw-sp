#include "DriftSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/MissionInstructionPage.hh"

namespace UI {

void DriftSelectPage::onActivate() {
    REPLACED(onActivate)();

    auto *sectionManager = SectionManager::Instance();
    selectDefault(m_buttons[sectionManager->globalContext()->m_driftModes[0] != 0]);
}

void DriftSelectPage::onButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto *sectionManager = SectionManager::Instance();
    auto *section = sectionManager->currentSection();
    auto sectionId = section->id();

    switch (button->m_index) {
    case 0:
    case 1:
        sectionManager->registeredPadManager().setDriftIsAuto(0, button->m_index);
        System::SaveManager::Instance()->setSetting<SP::ClientSettings::Setting::DriftMode>(
                static_cast<SP::ClientSettings::DriftMode>(button->m_index));
        sectionManager->saveManagerProxy()->markLicensesDirty();
        sectionManager->globalContext()->m_driftModes[0] = button->m_index;
        if (Section::GetSceneId(sectionId) == 4 /* Globe */) {
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
