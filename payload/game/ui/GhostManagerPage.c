#include "GhostManagerPage.h"

#include "SectionManager.h"

#include "../system/RaceConfig.h"
#include "../system/SaveManager.h"

#include <stdlib.h>
#include <string.h>

void GhostList_populate(GhostList *this, u32 courseId);

static void my_GhostList_populate(GhostList *UNUSED(this), u32 UNUSED(courseId)) {}
PATCH_B(GhostList_populate, my_GhostList_populate);

void GhostManagerPage_requestPopulate(GhostManagerPage *this) {
    this->nextRequest = GHOST_MANAGER_PAGE_REQUEST_POPULATE;
}

void GhostManagerPage_processPopulate(GhostManagerPage *this);

void GhostManagerPage_dispatchPopulate(GhostManagerPage *this) {
    u32 courseId = s_raceConfig->menuScenario.courseId;
    if (SaveManager_computeCourseSha1Async(s_saveManager, courseId)) {
        GhostManagerPage_processPopulate(this);
        return;
    }

    this->currentRequest = GHOST_MANAGER_PAGE_REQUEST_POPULATE;
}

static int compareRawGhostHeadersByTime(const RawGhostHeader *h0, const RawGhostHeader *h1) {
    const RawTime *t0 = &h0->raceTime;
    const RawTime *t1 = &h1->raceTime;
    s32 d0 = (t0->minutes * 60 + t0->seconds) * 1000 + t0->milliseconds;
    s32 d1 = (t1->minutes * 60 + t1->seconds) * 1000 + t1->milliseconds;
    return d0 < d1 ? -1 : d0 > d1 ? 1 : 0;
}

static int compareRawGhostHeadersByDate(const RawGhostHeader *h0, const RawGhostHeader *h1) {
    s32 t0 = (h0->year * 12 + h0->month) * 31 + h0->day;
    s32 t1 = (h1->year * 12 + h1->month) * 31 + h1->day;
    return t0 < t1 ? -1 : t0 > t1 ? 1 : 0;
}

static int compareGhostIndices(const void *p0, const void *p1) {
    u16 i0 = *(u16 *)p0;
    u16 i1 = *(u16 *)p1;
    const RawGhostHeader *h0 = &s_saveManager->rawGhostHeaders[i0];
    const RawGhostHeader *h1 = &s_saveManager->rawGhostHeaders[i1];
    switch (SaveManager_getTaRuleGhostSorting(s_saveManager)) {
    case SP_TA_RULE_GHOST_SORTING_FASTEST:
        return compareRawGhostHeadersByTime(h0, h1);
    case SP_TA_RULE_GHOST_SORTING_SLOWEST:
        return compareRawGhostHeadersByTime(h1, h0);
    case SP_TA_RULE_GHOST_SORTING_NEWEST:
        return compareRawGhostHeadersByDate(h1, h0);
    case SP_TA_RULE_GHOST_SORTING_OLDEST:
        return compareRawGhostHeadersByDate(h0, h1);
    default:
        // Should be unreachable
        return 0;
    }
}

void GhostManagerPage_processPopulate(GhostManagerPage *this) {
    if (s_saveManager->isBusy) {
        return;
    }

    u32 courseId = s_raceConfig->menuScenario.courseId;
    const u8 *courseSha1 = SaveManager_getCourseSha1(s_saveManager, courseId);
    bool speedModIsEnabled = SaveManager_getTaRuleClass(s_saveManager) == SP_TA_RULE_CLASS_200CC;

    SpGhostList *list = &this->list;
    list->count = 0;
    for (u32 i = 0; i < s_saveManager->ghostCount; i++) {
        const RawGhostHeader *header = &s_saveManager->rawGhostHeaders[i];
        const GhostFooter *footer = &s_saveManager->ghostFooters[i];
        switch (footer->magic) {
        case CTGP_FOOTER_MAGIC:
        case SP_FOOTER_MAGIC:
            if (memcmp(GhostFooter_getCourseSha1(footer), courseSha1, 0x14)) {
                continue;
            }
            if (GhostFooter_hasSpeedMod(footer) != speedModIsEnabled) {
                continue;
            }
            break;
        default:
            if (courseId != header->courseId) {
                continue;
            }
        }

        list->indices[list->count++] = i;
    }

    qsort(list->indices, list->count, sizeof(u16), compareGhostIndices);

    this->currentRequest = GHOST_MANAGER_PAGE_REQUEST_NONE;
}

static void setupTimeAttack(bool isRace, bool fromReplay) {
    GlobalContext *cx = s_sectionManager->globalContext;
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    if (!isRace) {
        MiiGroup_copy(&cx->playerMiis, &cx->localPlayerMiis, 0, cx->timeAttackGhostCount);
    }
    for (u32 i = cx->timeAttackGhostCount; i --> 0;) {
        menuScenario->players[i + !!isRace].type = PLAYER_TYPE_GHOST;
        if (fromReplay) {
            MiiGroup_swap(&cx->playerMiis, i, i + 1);
        } else {
            RawGhostHeader *header = (RawGhostHeader *)(*menuScenario->ghostBuffer)[i];
            MiiGroup_insertFromRaw(&cx->playerMiis, i + !!isRace, &header->mii);
        }
    }
    menuScenario->courseId = cx->timeAttackCourseId;
    GlobalContext_copyPlayerMiis(cx);
    menuScenario->engineClass = ENGINE_CLASS_150CC;
    menuScenario->gameMode = GAME_MODE_TIME_ATTACK;
    menuScenario->gameType = GAME_TYPE_TIME_ATTACK;
    menuScenario->modeFlags &= ~MODE_FLAG_MIRROR;
    menuScenario->modeFlags &= ~MODE_FLAG_TEAMS;
}

void GhostManagerPage_setupGhostReplay(GhostManagerPage *this, bool isStaffGhost);

static void my_GhostManagerPage_setupGhostReplay(GhostManagerPage *UNUSED(this),
        bool UNUSED(isStaffGhost)) {
    setupTimeAttack(false, false);
}
PATCH_B(GhostManagerPage_setupGhostReplay, my_GhostManagerPage_setupGhostReplay);

void GhostManagerPage_setupGhostRace(GhostManagerPage *this, bool isStaffGhost, bool isNewRecord,
        bool fromReplay);

static void my_GhostManagerPage_setupGhostRace(GhostManagerPage *UNUSED(this),
        bool UNUSED(isStaffGhost), bool UNUSED(isNewRecord), bool fromReplay) {
    setupTimeAttack(true, fromReplay);
}
PATCH_B(GhostManagerPage_setupGhostRace, my_GhostManagerPage_setupGhostRace);
