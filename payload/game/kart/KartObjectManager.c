#include "KartObjectManager.h"

#include "game/effect/EffectManager.h"
#include "game/system/GhostFile.h"
#include "game/system/RaceConfig.h"
#include "game/system/RaceManager.h"
#include "game/system/SaveManager.h"
#include "game/ui/page/RacePage.h"

#include <sp/settings/ClientSettings.h>

bool speedModIsEnabled;
f32 speedModFactor;
f32 speedModReverseFactor;

extern f32 minDriftSpeedFactor;
extern f32 boostAccelerations[6];
extern f32 ai_808cb550;

u8 s_playerDrawPriorities[12];

KartObjectManager *KartObjectManager_ct(KartObjectManager *this);

static void my_KartObjectManager_createInstance(void) {
    switch (s_raceConfig->raceScenario.gameMode) {
    case GAME_MODE_OFFLINE_VS:
    case GAME_MODE_BATTLE:
        speedModIsEnabled = vsSpeedModIsEnabled;
        break;
    case GAME_MODE_TIME_ATTACK:
        speedModIsEnabled = SaveManager_GetTAClass() == kTaRuleClass_200cc;
        break;
    default:
        speedModIsEnabled = false;
    }
    speedModFactor = speedModIsEnabled ? 1.5f : 1.0f;
    speedModReverseFactor = 1.0f / speedModFactor;

    minDriftSpeedFactor = 0.55f / speedModFactor;
    boostAccelerations[0] = 3.0f * speedModFactor;
    ai_808cb550 = 70.0f * speedModFactor;

    u32 courseId = s_raceConfig->raceScenario.courseId;
    const u8 *courseSha1 = SaveManager_CourseSHA1(courseId);
    bool isVanilla = SaveManager_GetVanillaMode() == kVanillaMode_Enable;
    if (s_raceConfig->raceScenario.playerCount > 2) {
        isVanilla = false;
    }

    SPFooter_OnRaceStart(courseSha1, speedModIsEnabled, isVanilla,
            s_raceConfig->raceScenario.modeFlags);

    s_kartObjectManager = new (sizeof(KartObjectManager));
    KartObjectManager_ct(s_kartObjectManager);
}
PATCH_B(KartObjectManager_createInstance, my_KartObjectManager_createInstance);

static bool playerIsSolid(u32 playerId) {
    const RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
    if (raceScenario->players[playerId].type != PLAYER_TYPE_GHOST) {
        return true;
    }

    switch (SaveManager_GetTASolidGhosts()) {
    case kTaRuleSolidGhosts_None:
        return false;
    case kTaRuleSolidGhosts_All:
        return true;
    default:
        return playerId == s_racePage->watchedPlayerId;
    }
}

enum {
    SOUND_SETTING_NONE = 0x0,
    SOUND_SETTING_PARTIAL = 0x1,
    SOUND_SETTING_FULL = 0x2,
};

static u32 getGhostSoundSetting(u32 playerId) {
    switch (SaveManager_GetTAGhostSound()) {
    case kTaRuleGhostSound_None:
        return SOUND_SETTING_NONE;
    case kTaRuleGhostSound_All:
        if (playerId != s_racePage->watchedPlayerId) {
            return SOUND_SETTING_PARTIAL;
        }
        return SOUND_SETTING_FULL;
    default:
        if (playerId != s_racePage->watchedPlayerId) {
            return SOUND_SETTING_NONE;
        }
        return SOUND_SETTING_FULL;
    }
}

void KartObjectManager_beforeCalc(KartObjectManager *this) {
    for (u32 i = 0; i < this->count; i++) {
        s_playerDrawPriorities[i] = playerIsSolid(i) ? 0x4e : 0x3;
    }

    for (u32 i = 0; i < this->count; i++) {
        const RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
        if (raceScenario->players[i].type != PLAYER_TYPE_GHOST) {
            continue;
        }

        u32 soundSetting = getGhostSoundSetting(i);
        bool hasFinished = s_raceManager->players[i]->hasFinished;
        KartAccessor *accessor = this->objects[i]->accessor;
        accessor->sound->isLocal = soundSetting == SOUND_SETTING_FULL;
        accessor->sound->isGhost = soundSetting == SOUND_SETTING_NONE;
        accessor->driver->sound->isLocal = soundSetting == SOUND_SETTING_FULL && !hasFinished;
        accessor->driver->sound->isGhost = soundSetting == SOUND_SETTING_NONE || hasFinished;
    }

    for (u32 i = 0; i < this->count; i++) {
        const RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
        if (raceScenario->players[i].type != PLAYER_TYPE_GHOST) {
            continue;
        }

        KartEffect *effect = s_effectManager->karts[i];
        effect->isGhost = i != s_racePage->watchedPlayerId;
    }
}
