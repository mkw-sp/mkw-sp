#include "DriftSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/MissionInstructionPage.hh"

#include <sp/settings/ClientSettings.hh>
#include <sp/CourseDatabase.hh>

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
            auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
            if (menuScenario.gameMode == System::RaceConfig::GameMode::Mission) {
                auto *missionInstructionPage = section->page<PageId::MissionInstruction>();
                if (missionInstructionPage->levelId() == 7 /* Boss */) {
                    menuScenario.cameraMode = 5;
                    requestChangeSection(SectionId::MRBossDemo, button);
                } else {
                    requestChangeSection(SectionId::MR, button);
                }
            } else if (selectRandomCourse(menuScenario)) {
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

bool DriftSelectPage::selectRandomCourse(System::RaceConfig::Scenario &menuScenario) {
    auto *saveManager = System::SaveManager::Instance();

    SP::CourseDatabase::Filter filter;
    SP::ClientSettings::CourseSelection setting;
    if (menuScenario.gameMode == System::RaceConfig::GameMode::OfflineVS) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::VSCourseSelection>();
        filter.battle = false;
        filter.race = true;
    } else if (menuScenario.gameMode == System::RaceConfig::GameMode::OfflineBT) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::BTCourseSelection>();
        filter.battle = true;
        filter.race = false;
    } else {
        return false;
    }

    if (setting != SP::ClientSettings::CourseSelection::Random) {
        return false;
    }

    auto &courseDatabase = SP::CourseDatabase::Instance();
    auto courseCount = courseDatabase.count(filter);
    auto courseIdx = hydro_random_uniform(courseCount) - 1;

    menuScenario.courseId = courseDatabase.entry(filter, courseIdx).courseId;
    return true;
}

} // namespace UI
