#include "KartObjectManager.hh"

#include "game/effect/EffectManager.hh"
#include "game/kart/PlayerModel.hh"
#include "game/sound/KartSound.hh"
#include "game/system/GhostFile.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/page/RacePage.hh"

#include <sp/settings/ClientSettings.hh>

bool speedModIsEnabled;
f32 speedModFactor;
f32 speedModReverseFactor;
u8 s_playerDrawPriorities[12];

extern f32 minDriftSpeedFactor;
extern f32 boostAccelerations[6];
extern f32 ai_808cb550;

namespace Kart {

enum class SoundSetting {
    None = 0x0,
    Partial = 0x1,
    Full = 0x2,
};

bool playerIsSolid(u32 playerId) {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.players[playerId].type != System::RaceConfig::Player::Type::Ghost) {
        return true;
    }

    auto *saveManager = System::SaveManager::Instance();
    switch (saveManager->getSetting<SP::ClientSettings::Setting::TASolidGhosts>()) {
    case SP::ClientSettings::TASolidGhosts::None:
        return false;
    case SP::ClientSettings::TASolidGhosts::All:
        return true;
    default:
        auto *racePage = UI::RacePage::Instance();
        return playerId == racePage->watchedPlayerId();
    }
}

SoundSetting getGhostSoundSetting(u32 playerId) {
    auto *saveManager = System::SaveManager::Instance();
    auto watchedPlayerId = UI::RacePage::Instance()->watchedPlayerId();

    switch (saveManager->getSetting<SP::ClientSettings::Setting::TAGhostSound>()) {
    case SP::ClientSettings::TAGhostSound::None:
        return SoundSetting::None;
    case SP::ClientSettings::TAGhostSound::All:
        if (playerId != watchedPlayerId) {
            return SoundSetting::Partial;
        }
        return SoundSetting::Full;
    default:
        if (playerId != watchedPlayerId) {
            return SoundSetting::None;
        }
        return SoundSetting::Full;
    }
}

void KartObjectManager::beforeCalc() {
    for (u32 i = 0; i < m_count; i++) {
        s_playerDrawPriorities[i] = playerIsSolid(i) ? 0x4e : 0x3;
    }

    auto *raceManager = System::RaceManager::Instance();
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    for (u32 i = 0; i < m_count; i++) {
        if (raceScenario.players[i].type != System::RaceConfig::Player::Type::Ghost) {
            continue;
        }

        auto soundSetting = getGhostSoundSetting(i);
        auto *kartSound = m_objects[i]->getKartSound();
        auto *kartModel = m_objects[i]->getPlayerModel();
        bool hasFinished = raceManager->player(i)->hasFinished();

        kartSound->m_isLocal = soundSetting == SoundSetting::Full;
        kartSound->m_isGhost = soundSetting == SoundSetting::None;
        kartModel->sound->isLocal = soundSetting == SoundSetting::Full && !hasFinished;
        kartModel->sound->isGhost = soundSetting == SoundSetting::None || hasFinished;
    }

    auto *racePage = UI::RacePage::Instance();
    for (u32 i = 0; i < m_count; i++) {
        if (raceScenario.players[i].type != System::RaceConfig::Player::Type::Ghost) {
            continue;
        }

        auto *effect = Effect::EffectManager::Instance()->getKartEffect(i);
        effect->m_isGhost = i != racePage->watchedPlayerId();
    }
}

void KartObjectManager::CreateInstance() {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    auto *saveManager = System::SaveManager::Instance();

    auto taClass = saveManager->getSetting<SP::ClientSettings::Setting::TAClass>();
    auto vsClass = saveManager->getSetting<SP::ClientSettings::Setting::VSClass>();

    switch (raceScenario.gameMode) {
    case System::RaceConfig::GameMode::OfflineVS:
        speedModIsEnabled = vsClass == SP::ClientSettings::EngineClass::CC200;
        break;
    case System::RaceConfig::GameMode::TimeAttack:
        speedModIsEnabled = taClass == SP::ClientSettings::TAClass::CC200;
        break;
    default:
        speedModIsEnabled = false;
    }

    speedModFactor = speedModIsEnabled ? 1.5f : 1.0f;
    speedModReverseFactor = 1.0f / speedModFactor;

    minDriftSpeedFactor = 0.55f / speedModFactor;
    boostAccelerations[0] = 3.0f * speedModFactor;
    ai_808cb550 = 70.0f * speedModFactor;

    auto value = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
    bool isVanilla = value == SP::ClientSettings::VanillaMode::Enable;
    if (raceScenario.playerCount > 2) {
        isVanilla = false;
    }

    System::SPFooter::OnRaceStart(speedModIsEnabled, isVanilla, raceScenario.mirror);

    s_instance = new KartObjectManager;
}

KartObjectManager *KartObjectManager::Instance() {
    return s_instance;
}

extern "C" {
void KartObjectManager_beforeCalc(KartObjectManager *self) {
    self->beforeCalc();
}
}

} // namespace Kart
