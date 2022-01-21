#include "GhostManagerPage.h"

#include "SectionManager.h"

#include "../system/RaceConfig.h"

int compareGhostEntries(const void *p0, const void *p1);

int my_compareGhostEntries(const void *p0, const void *p1) {
    const GhostEntry *e0 = p0;
    const GhostEntry *e1 = p1;
    const Time *t0 = &e0->file->raceTime;
    const Time *t1 = &e1->file->raceTime;
    s32 d0 = ((t0->minutes * 60) + t0->seconds) * 1000 + t0->milliseconds;
    s32 d1 = ((t1->minutes * 60) + t1->seconds) * 1000 + t1->milliseconds;
    return d0 - d1;
}

PATCH_B(compareGhostEntries, my_compareGhostEntries);

static void setupTimeAttack(bool isRace, bool fromReplay) {
    GlobalContext *cx = s_sectionManager->globalContext;
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    for (u32 i = cx->timeAttackGhostCount; i --> 0;) {
        menuScenario->players[i + !!isRace].type = PLAYER_TYPE_GHOST;
        if (fromReplay) {
            MiiGroup_copy(&cx->playerMiis, &cx->playerMiis, i, i + !!isRace);
        } else {
            RawGhostHeader *header = (RawGhostHeader *)(*menuScenario->ghostBuffer)[i];
            MiiGroup_insertFromRaw(&cx->playerMiis, i + !!isRace, &header->mii);
        }
    }
    if (fromReplay) {
        MiiGroup_copy(&cx->playerMiis, &cx->playerMiis, 11, 0);
    }
    menuScenario->courseId = cx->timeAttackCourseId;
    GlobalContext_copyPlayerMiis(cx);
    menuScenario->engineClass = ENGINE_CLASS_150CC;
    menuScenario->gameMode = GAME_MODE_TIME_ATTACK;
    menuScenario->gameType = GAME_TYPE_TIME_ATTACK;
    menuScenario->modeFlags &= ~MODE_FLAG_MIRROR;
    menuScenario->modeFlags &= ~MODE_FLAG_TEAMS;
}

void my_GhostManagerPage_setupGhostReplay(GhostManagerPage *this, bool isStaffGhost) {
    UNUSED(this);
    UNUSED(isStaffGhost);

    setupTimeAttack(false, false);
}

PATCH_B(GhostManagerPage_setupGhostReplay, my_GhostManagerPage_setupGhostReplay);

void my_GhostManagerPage_setupGhostRace(GhostManagerPage *this, bool isStaffGhost, bool isNewRecord, bool fromReplay) {
    UNUSED(this);
    UNUSED(isStaffGhost);
    UNUSED(isNewRecord);

    setupTimeAttack(true, fromReplay);
}

PATCH_B(GhostManagerPage_setupGhostRace, my_GhostManagerPage_setupGhostRace);
