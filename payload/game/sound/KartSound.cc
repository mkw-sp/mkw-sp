#include "KartSound.hh"

#include "game/sound/ItemMusicManager.hh"
#include "game/sound/RaceSoundManager.hh"
#include "game/sound/SceneSoundManager.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/page/RacePage.hh"

namespace Sound {

void KartSound::calcLap() {
    if (m_proxy->isCPU()) {
        REPLACED(calcLap)();
        return;
    }

    auto *saveManager = System::SaveManager::Instance();
    u32 playerId = m_proxy->getPlayerId();

    u8 maxLap = System::RaceManager::Instance()->player(playerId)->maxLap();
    if (maxLap != m_maxLap) {
        if (maxLap != System::RaceManager::GetLapCount()) {
            if (!m_proxy->isGhost() || playerId == UI::RacePage::Instance()->watchedPlayerId()) {
                SceneSoundManager::Instance()->play(SoundId::SE_RC_LAP, -1);
            }
        } else if (!m_proxy->isGhost()) {
            u32 state = RaceSoundManager::Instance()->state();
            auto setting = saveManager->getSetting<SP::ClientSettings::Setting::LastLapSpeedup>();
            if ((state == 4 || state == 6) && setting != SP::ClientSettings::LastLapSpeedup::None) {
                if (setting == SP::ClientSettings::LastLapSpeedup::Static) {
                    RaceSoundManager::Instance()->transition(5);
                } else {
                    SceneSoundManager::Instance()->play(SoundId::W_BGM_FINALLAP_FAN, -1);
                    if (setting == SP::ClientSettings::LastLapSpeedup::Dynamic) {
                        ItemMusicManager::Instance()->m_speedup = true;
                    }
                }
            }
        } else if (playerId == UI::RacePage::Instance()->watchedPlayerId()) {
            auto setting = saveManager->getSetting<SP::ClientSettings::Setting::LastLapJingle>();
            SoundId soundId = SoundId::SE_RC_LAP;
            if (setting == SP::ClientSettings::LastLapJingle::Always) {
                soundId = SoundId::W_BGM_FINALLAP_FAN;
            }
            SceneSoundManager::Instance()->play(soundId, -1);
        }

        m_maxLap = maxLap;
    }

    bool hasFinished = System::RaceManager::Instance()->player(playerId)->hasFinished();
    if (hasFinished && !m_hasFinished) {
        if (m_proxy->isGhost()) {
            if (playerId == UI::RacePage::Instance()->watchedPlayerId()) {
                SceneSoundManager::Instance()->play(SoundId::SE_RC_GOAL, -1);
            }

            m_hasFinished = hasFinished;
        }
    }

    // The original code will handle the 4th case (local player finishing) for us
    REPLACED(calcLap)();
}

} // namespace Sound
