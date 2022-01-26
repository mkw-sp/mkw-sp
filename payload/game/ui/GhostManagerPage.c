#include "GhostManagerPage.h"

#include "SectionManager.h"

#include "../system/RaceConfig.h"
#include "../system/SaveManager.h"

#include <stdlib.h>
#include <string.h>

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

static int compareGhostEntriesByTime(const GhostEntry *e0, const GhostEntry *e1) {
    const Time *t0 = &e0->file->raceTime;
    const Time *t1 = &e1->file->raceTime;
    s32 d0 = (t0->minutes * 60 + t0->seconds) * 1000 + t0->milliseconds;
    s32 d1 = (t1->minutes * 60 + t1->seconds) * 1000 + t1->milliseconds;
    return d0 < d1 ? -1 : d0 > d1 ? 1 : 0;
}

static int compareGhostEntriesByDate(const GhostEntry *e0, const GhostEntry *e1) {
    const GhostFile *f0 = e0->file;
    const GhostFile *f1 = e1->file;
    s32 t0 = (f0->year * 12 + f0->month) * 31 + f0->day;
    s32 t1 = (f1->year * 12 + f1->month) * 31 + f1->day;
    return t0 < t1 ? -1 : t0 > t1 ? 1 : 0;
}

static int compareGhostEntries(const void *p0, const void *p1) {
    const GhostEntry *e0 = p0;
    const GhostEntry *e1 = p1;
    switch (SaveManager_getTaRuleGhostSorting(s_saveManager)) {
    case SP_TA_RULE_GHOST_SORTING_FASTEST:
        return compareGhostEntriesByTime(e0, e1);
    case SP_TA_RULE_GHOST_SORTING_SLOWEST:
        return compareGhostEntriesByTime(e1, e0);
    case SP_TA_RULE_GHOST_SORTING_NEWEST:
        return compareGhostEntriesByDate(e1, e0);
    case SP_TA_RULE_GHOST_SORTING_OLDEST:
        return compareGhostEntriesByDate(e0, e1);
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
    const u32 *courseSha1 = SaveManager_getCourseSha1(s_saveManager, courseId);
    bool speedModIsEnabled = SaveManager_getTaRuleClass(s_saveManager) == SP_TA_RULE_CLASS_200CC;

    GhostList *list = &this->list;
    list->count = 0;
    const GhostGroup *group = this->groups[4];
    for (u32 i = 0; i < group->count && list->count < ARRAY_SIZE(list->entries); i++) {
        const GhostFile *file = GhostGroup_get(group, i);
        if (!file) {
            continue;
        }

        const GhostFooter *footer = &s_saveManager->ghostFooters[i];
        switch (footer->magic) {
        case CTGP_FOOTER_MAGIC:
        case SP_FOOTER_MAGIC:
            if (memcmp(GhostFooter_getCourseSha1(footer), courseSha1, 5 * sizeof(u32))) {
                continue;
            }
            if (GhostFooter_hasSpeedMod(footer) != speedModIsEnabled) {
                continue;
            }
            break;
        default:
            if (courseId != file->courseId) {
                continue;
            }
        }

        list->entries[list->count].file = file;
        list->entries[list->count].groupIndex = 4;
        list->entries[list->count].index = i;
        list->entries[list->count].isNew = false;
        list->count++;
    }

    qsort(list->entries, list->count, sizeof(GhostEntry), compareGhostEntries);

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
