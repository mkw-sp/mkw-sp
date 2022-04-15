#include "CtrlRaceNameBalloon.h"

#include "../page/RacePage.h"

#include "../SectionManager.h"

#include "../../race/RaceGlobals.h"

#include "../../system/RaceConfig.h"
#include "../../system/SaveManager.h"

static BalloonManager *my_BalloonManager_ct(BalloonManager *this) {
    this->nameCount = 0;
    this->namePositions = spAllocArray(12, sizeof(Vec3), 0x4, NULL);
    for (u32 i = 0; i < 12; i++) {
        this->namePositions[i] = (Vec3) { 0 };
    }

    return this;
}
PATCH_B(BalloonManager_ct, my_BalloonManager_ct);

static void my_BalloonManager_dt(BalloonManager *this, s32 type) {
    spFree(this->namePositions);

    if (type > 0) {
        delete(this);
    }
}
PATCH_B(BalloonManager_dt, my_BalloonManager_dt);

static void my_BalloonManager_init(BalloonManager *this, u8 localPlayerId) {
    this->localPlayerId = localPlayerId;
    for (u32 i = 0; i < ARRAY_SIZE(this->playerIds); i++) {
        this->playerIds[i] = -1;
    }
    for (u32 i = 0; i < ARRAY_SIZE(this->nameIsEnabled); i++) {
        this->nameIsEnabled[i] = false;
    }
}
PATCH_B(BalloonManager_init, my_BalloonManager_init);

static void my_BalloonManager_addNameControl(BalloonManager *this, CtrlRaceNameBalloon *UNUSED(nameControl)) {
    this->nameCount++;
}
PATCH_B(BalloonManager_addNameControl, my_BalloonManager_addNameControl);

static void CtrlRaceNameBalloon_refreshTextName(CtrlRaceNameBalloon *this, u32 playerId) {
    MessageInfo info = {
        .miis[0] = MiiGroup_get(&s_sectionManager->globalContext->playerMiis, playerId),
    };
    LayoutUIControl_setMessage(this, "chara_name", 9501, &info);
}

static void CtrlRaceNameBalloon_refreshTextTime(CtrlRaceNameBalloon *this, u32 playerId, bool leadingZeroes) {
    const RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
    u32 index = raceScenario->players[0].type == PLAYER_TYPE_GHOST ? playerId : playerId - 1;
    const RawGhostHeader *header = (RawGhostHeader *)(*raceScenario->ghostBuffer)[index];
    MessageInfo info = {
        .intVals[0] = header->raceTime.minutes,
        .intVals[1] = header->raceTime.seconds,
        .intVals[2] = header->raceTime.milliseconds,
    };
    LayoutUIControl_setMessage(this, "chara_name", leadingZeroes ? 1400 : 10071, &info);
}

static void CtrlRaceNameBalloon_refreshTextDate(CtrlRaceNameBalloon *this, u32 playerId) {
    const RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
    u32 index = raceScenario->players[0].type == PLAYER_TYPE_GHOST ? playerId : playerId - 1;
    const RawGhostHeader *header = (RawGhostHeader *)(*raceScenario->ghostBuffer)[index];
    MessageInfo info = {
        .intVals[0] = header->year + 2000,
        .intVals[1] = header->month,
        .intVals[2] = header->day,
    };
    LayoutUIControl_setMessage(this, "chara_name", 10048, &info);
}

void CtrlRaceNameBalloon_refreshText(CtrlRaceNameBalloon *this, u32 playerId) {
    u32 playerType = s_raceConfig->raceScenario.players[playerId].type;
    if (playerType != PLAYER_TYPE_GHOST) {
        CtrlRaceNameBalloon_refreshTextName(this, playerId);
        return;
    }

    switch (SaveManager_getSetting(s_saveManager, kSetting_TaRuleGhostTagContent)) {
    case kTaRuleGhostTagContent_Name:
        CtrlRaceNameBalloon_refreshTextName(this, playerId);
        break;
    case kTaRuleGhostTagContent_Time:
        CtrlRaceNameBalloon_refreshTextTime(this, playerId, /* leadingZeroes */ true);
        break;
    case kTaRuleGhostTagContent_TimeNoLeading:
        CtrlRaceNameBalloon_refreshTextTime(this, playerId, /* leadingZeroes */ false);
        break;
    case kTaRuleGhostTagContent_Date:
        CtrlRaceNameBalloon_refreshTextDate(this, playerId);
        break;
    }
}

void CtrlRaceNameBalloon_calcVisibility(CtrlRaceNameBalloon *this) {
    // An artificial 1 frame delay is added to match the camera.
    u8 lastWatchedPlayerId = this->lastWatchedPlayerId;
    RacePage *page = (RacePage *)this->group->page;
    this->lastWatchedPlayerId = page->watchedPlayerId;

    if (this->isHidden) {
        return;
    }

    if (!s_raceGlobals.isTimeAttack) {
        return;
    }

    switch (SaveManager_getSetting(s_saveManager, kSetting_TaRuleGhostTagVisibility)) {
    case kTaRuleGhostTagVisibility_None:
        this->isHidden = true;
        return;
    case kTaRuleGhostTagVisibility_Watched:
        this->isHidden = this->playerId != (s32)lastWatchedPlayerId;
        return;
    default:
        return;
    }
}
