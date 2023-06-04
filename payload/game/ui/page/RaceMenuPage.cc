#include "RaceMenuPage.hh"

#include "game/sound/util/BackgroundMusicManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

#include <sp/CourseDatabase.hh>
#include <sp/SaveStateManager.hh>
#include <sp/settings/ClientSettings.hh>

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

namespace UI {

void RaceMenuPage::onButtonFront(PushButton *button, u32 localPlayerId) {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();

    auto buttonId = static_cast<ButtonId>(button->m_index);
    switch (buttonId) {
    case ButtonId::Restart1:
    case ButtonId::Restart3:
    case ButtonId::BattleGhost:
        if (raceScenario.gameMode == System::RaceConfig::GameMode::TimeAttack) {
            System::RaceConfig::Instance()->applyEngineClass();
        }
        REPLACED(onButtonFront)(button, localPlayerId);
        return;
    case ButtonId::Next:
        if (menuScenario.mirrorRng) {
            menuScenario.mirror = hydro_random_uniform(20) >= 17;
        }

        if (raceScenario.gameMode == System::RaceConfig::GameMode::OfflineVS ||
                raceScenario.gameMode == System::RaceConfig::GameMode::OfflineBT) {
            onNextButtonFront(button, localPlayerId);
            break;
        } else {
            REPLACED(onButtonFront)(button, localPlayerId);
            return;
        }
    case ButtonId::Settings:
        onSettingsButtonFront(button, localPlayerId);
        break;
    case ButtonId::ChangeGhostData:
        onChangeGhostDataButtonFront(button, localPlayerId);
        break;
    case ButtonId::SaveState:
        if (auto *saveStateManager = SP::SaveStateManager::Instance()) {
            saveStateManager->save();
        }
        break;
    case ButtonId::LoadState:
        if (auto *saveStateManager = SP::SaveStateManager::Instance()) {
            saveStateManager->reload();
        }
        break;
    default:
        REPLACED(onButtonFront)(button, localPlayerId);
        return;
    }
}

void RaceMenuPage::onNextButtonFront(PushButton *button, u32 /* localPlayerId */) {
    System::RaceConfig::Instance()->endRace();

    playSound(Sound::SoundId::SE_RC_PAUSE_EXIT_GAME, -1);

    SectionId sectionId;
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    if (IsLastMatch()) {
        bool isWin = false;
        for (u32 playerId = 0; playerId < menuScenario.localPlayerCount; playerId++) {
            u32 threshold = menuScenario.spMaxTeamSize * (1 + menuScenario.draw);
            if (threshold == 1) {
                threshold = 3;
            }
            if (menuScenario.players[playerId].rank <= threshold) {
                isWin = true;
            }
        }
        if (menuScenario.spMaxTeamSize >= 2 && menuScenario.draw) {
            if (!isWin) {
                menuScenario.draw = 0;
            }
            isWin = false;
        }
        if (menuScenario.isBattle()) {
            sectionId = SectionId::AwardsBT;
        } else {
            sectionId = SectionId::AwardsVS;
        }
        menuScenario.cameraMode = isWin ? 8 : 12;
        menuScenario.courseId = isWin ? Registry::Course::WinDemo : Registry::Course::LoseDemo;
        menuScenario.gameMode = System::RaceConfig::GameMode::Awards;
    } else {
        auto *saveManager = System::SaveManager::Instance();
        auto &courseDatabase = SP::CourseDatabase::Instance();

        menuScenario.cameraMode = 5;

        SP::ClientSettings::CourseSelection setting;
        SectionId nextSelectSection;
        if (menuScenario.isBattle()) {
            setting = saveManager->getSetting<SP::ClientSettings::Setting::BTCourseSelection>();
            nextSelectSection = SectionId::SingleSelectBTCourse;
            sectionId = SectionId::BTDemo;
        } else {
            setting = saveManager->getSetting<SP::ClientSettings::Setting::VSCourseSelection>();
            nextSelectSection = SectionId::SingleSelectVSCourse;
            sectionId = SectionId::VSDemo;
        };

        SP::CourseDatabase::Filter filter;
        filter.battle = menuScenario.isBattle();
        filter.race = !filter.battle;

        if (setting == SP::ClientSettings::CourseSelection::InOrder) {
            SP::CourseDatabase::Filter fullFilter;
            fullFilter.battle = true;
            fullFilter.race = true;

            std::optional<Registry::Course> nextCourse = std::nullopt;
            bool foundCurrentCourse = false;
            for (u8 i = 0; i < courseDatabase.totalCount(); i += 1) {
                auto entry = courseDatabase.entry(fullFilter, i);
                if (entry.courseId == menuScenario.courseId) {
                    foundCurrentCourse = true;
                } else if (foundCurrentCourse && entry.battle == menuScenario.isBattle()) {
                    nextCourse = entry.courseId;
                    break;
                }
            }

            if (!nextCourse) {
                menuScenario.courseId = courseDatabase.entry(filter, 0).courseId;
            } else {
                menuScenario.courseId = *nextCourse;
            }
        } else if (setting == SP::ClientSettings::CourseSelection::Random) {
            auto courseCount = courseDatabase.count(filter);
            auto courseIdx = hydro_random_uniform(courseCount);
            menuScenario.courseId = courseDatabase.entry(filter, courseIdx).courseId;
        } else {
            sectionId = nextSelectSection;
        }
    }

    f32 delay = button->getDelay();
    changeSection(sectionId, Anim::Next, delay);

    Sound::BackgroundMusicManager::Instance()->prepare(Section::GetSoundId(sectionId), true);
}

void RaceMenuPage::onSettingsButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuSettingsPage = section->page<PageId::MenuSettings>();
    menuSettingsPage->configure(nullptr, id());
    setReplacement(PageId::MenuSettings);
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void RaceMenuPage::onChangeGhostDataButtonFront(PushButton *button, u32 /* localPlayerId */) {
    playSound(Sound::SoundId::SE_RC_PAUSE_EXIT_GAME, -1);

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.cameraMode = 0;
    for (u32 i = 1; i < 12; i++) {
        menuScenario.players[i].type = System::RaceConfig::Player::Type::None;
    }

    f32 delay = button->getDelay();
    changeSection(SectionId::SingleChangeGhostData, Anim::Next, delay);
}

bool RaceMenuPage::IsLastMatch() {
    auto raceScenario = System::RaceConfig::Instance()->raceScenario();

    if (raceScenario.isBattle()) {
        auto *saveManager = System::SaveManager::Instance();
        s32 maxRaceCount = saveManager->getSetting<SP::ClientSettings::Setting::BTRaceCount>();

        return maxRaceCount <= (raceScenario.raceNumber + 1);
    }

    return REPLACED(IsLastMatch)();
}

} // namespace UI

extern "C" {
// Referenced by RaceMenuPage.S
const char *sButtonStrings[] = {
        "ButtonContinue",          // ButtonId::Continue1
        "ButtonQuit",              // ButtonId::Quit1
        "ButtonRestart",           // ButtonId::Restart1
        "ButtonRestart",           // ButtonId::Restart2
        "ButtonReplay",            // ButtonId::Replay
        "ButtonChangeCourse",      // ButtonId::ChangeCourse
        "ButtonChangeCharacter",   // ButtonId::ChangeCharacter
        "ButtonNext",              // ButtonId::Next
        "ButtonRanking",           // ButtonId::Ranking
        "ButtonContinueReplay",    // ButtonId::ContinueReplay
        "ButtonRestartReplay",     // ButtonId::RestartReplay
        "ButtonQuitReplay",        // ButtonId::QuitReplay
        "ButtonContinue",          // ButtonId::Continue2
        "ButtonQuit",              // ButtonId::Quit2
        "ButtonBattleGhost",       // ButtonId::BattleGhost
        "ButtonRestart",           // ButtonId::Restart3
        "ButtonContinue",          // ButtonId::Continue3
        "ButtonQuit",              // ButtonId::Quit3
        "ButtonChangeMission",     // ButtonId::ChangeMission
        "ButtonSend",              // ButtonId::Send1
        "ButtonNoSend",            // ButtonId::NoSend1
        "ButtonGoRanking",         // ButtonId::GoRanking
        "ButtonNotGoRanking",      // ButtonId::NotGoRanking
        "ButtonConfirmContinue",   // ButtonId::ConfirmContinue
        "ButtonConfirmQuit",       // ButtonId::ConfirmQuit
        "ButtonSendRecord",        // ButtonId::SendRecord
        "ButtonSend",              // ButtonId::Send2
        "ButtonNoSend",            // ButtonId::NoSend2
        "ButtonFriendGhostBattle", // ButtonId::stBattle
        "ButtonGoFriendRoom",      // ButtonId::GoFriendRoom
        "ButtonNotGoFriendRoom",   // ButtonId::NotGoFriendRoom
        "ButtonNextGhost",         // ButtonId::NextGhost
        "ButtonYes",               // ButtonId::Yes1
        "ButtonNo",                // ButtonId::No1
        "ButtonQuit",              // ButtonId::Quit4
        "ButtonYes",               // ButtonId::Yes2
        "ButtonNo",                // ButtonId::No2

        "ButtonSettings",        // ButtonId::LicenseSettings
        "ButtonChangeGhostData", // ButtonId::ChangeGhostData
        "ButtonSaveState",       // ButtonId::SaveState
        "ButtonLoadState",       // ButtonId::LoadState
};
}
