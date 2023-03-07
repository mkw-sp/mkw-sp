#include "RaceMenuPage.hh"

#include "game/sound/util/BackgroundMusicManager.hh"
#include "game/system/SaveManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

#include <sp/SaveStateManager.hh>
#include <sp/settings/ClientSettings.hh>

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

namespace UI {

void RaceMenuPage::onButtonFront(PushButton *button,
        u32 localPlayerId) {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();

    auto buttonId = static_cast<ButtonId>(button->m_index);
    switch (buttonId) {
    case ButtonId::Next:
        if (menuScenario.mirrorRng) {
            menuScenario.mirror = hydro_random_uniform(20) >= 17;
        }

        if (
            raceScenario.gameMode == System::RaceConfig::GameMode::OfflineVS ||
            raceScenario.gameMode == System::RaceConfig::GameMode::OfflineBT
        ) {
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

void RaceMenuPage::onNextButtonFront(PushButton *button,
        u32 /* localPlayerId */) {
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
        menuScenario.courseId = isWin ? 0x37 : 0x38;
        menuScenario.gameMode = System::RaceConfig::GameMode::Awards;
    } else {
        menuScenario.cameraMode = 5;
        if (menuScenario.isBattle()) {
            sectionId = SectionId::SingleSelectBTCourse;
        } else {
            sectionId = SectionId::SingleSelectVSCourse;
        };
    }

    f32 delay = button->getDelay();
    changeSection(sectionId, Anim::Next, delay);

    Sound::BackgroundMusicManager::Instance()->prepare(Section::GetSoundId(sectionId), true);
}

void RaceMenuPage::onSettingsButtonFront(PushButton *button,
        u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuSettingsPage = section->page<PageId::MenuSettings>();
    menuSettingsPage->configure(nullptr, id());
    setReplacement(PageId::MenuSettings);
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void RaceMenuPage::onChangeGhostDataButtonFront(PushButton *button,
        u32 /* localPlayerId */) {
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
        auto maxRaceCount = saveManager->getSetting<SP::ClientSettings::Setting::BTRaceCount>();

        return maxRaceCount <= (raceScenario.raceNumber + 1);
    }

    return REPLACED(IsLastMatch)();
}

} // namespace UI
