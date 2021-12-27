#include "SaveManager.h"

#include "NandHelper.h"
#include "RaceConfig.h"
#include "RootScene.h"

#include "../ui/SectionManager.h"

#include <stdalign.h>
#include <stdio.h>
#include <string.h>

SaveManager *SaveManager_ct(SaveManager *this);

SaveManager *my_SaveManager_createInstance(void) {
    s_saveManager = new(sizeof(SaveManager));
    SaveManager_ct(s_saveManager);
    SaveManager *this = s_saveManager;

    this->ghostCount = 0;
    EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
    this->rawGhostHeaders = EGG_Heap_alloc(MAX_GHOST_COUNT * sizeof(RawGhostHeader), 0x4, heap);
    this->ghostPaths = EGG_Heap_alloc(MAX_GHOST_COUNT * NAND_MAX_PATH, 0x4, heap);

    for (u32 i = 0; i < ARRAY_SIZE(this->spLicenses); i++) {
        this->spLicenses[i] = NULL;
    }

    return s_saveManager;
}

PATCH_B(SaveManager_createInstance, my_SaveManager_createInstance);

static void SaveManager_initGhost(SaveManager *this, const char *path) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    alignas(0x20) RawGhostHeader raw;
    if (NandHelper_readFromFile(path, &raw, sizeof(raw), 0) != RK_NAND_RESULT_OK) {
        return;
    }

    if (!RawGhostHeader_isValid(&raw)) {
        return;
    }

    this->rawGhostHeaders[this->ghostCount] = raw;
    memcpy(&this->ghostPaths[this->ghostCount], path, NAND_MAX_PATH);
    this->ghostCount++;
}

static u32 SaveManager_initGhostsDir(SaveManager *this, const char *dir) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return RK_NAND_RESULT_OK;
    }

    if (strlen(dir) + 1 + NAND_MAX_NAME > NAND_MAX_PATH) {
        return RK_NAND_RESULT_OK;
    }

    u32 result, fileCount;

    result = NandHelper_readDir(dir, NULL, &fileCount);
    if (result != RK_NAND_RESULT_OK) {
        return result;
    }

    u32 size = OSRoundUp32B(fileCount * (NAND_MAX_NAME + 1));
    EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
    char *names = EGG_Heap_alloc(size, 0x20, heap);
    result = NandHelper_readDir(dir, names, &fileCount);
    if (result != RK_NAND_RESULT_OK) {
        goto cleanup;
    }

    const char *name = names;
    for (u32 i = 0; i < fileCount; i++) {
        char path[NAND_MAX_PATH];
        snprintf(path, sizeof(path), "%s/%s", dir, name);

        u32 type;
        result = NandHelper_getType(path, &type);
        if (result != RK_NAND_RESULT_OK) {
            goto cleanup;
        }

        switch (type) {
        case NAND_TYPE_FILE:
            SaveManager_initGhost(this, path);
            break;
        case NAND_TYPE_DIR:
            result = SaveManager_initGhostsDir(this, path);
            if (result != RK_NAND_RESULT_OK) {
                goto cleanup;
            }
            break;
        }

        name += strlen(name) + 1;
    }

cleanup:
    EGG_Heap_free(names, NULL);
    return result;
}

static u32 SaveManager_initGhosts(SaveManager *this) {
    u32 result;

    char dir[NAND_MAX_PATH];
    result = NandHelper_getHomeDir(dir);
    if (result != RK_NAND_RESULT_OK) {
        return result;
    }

    u32 offset = strlen(dir);
    u32 size = strlen("/ghosts") + 1;
    if (offset + size > NAND_MAX_PATH) {
        return RK_NAND_RESULT_OTHER;
    }
    memcpy(dir + offset, "/ghosts", size);

    result = NandHelper_createDir(dir, NAND_PERM_OWNER_READ | NAND_PERM_OWNER_WRITE);
    if (result != RK_NAND_RESULT_OK) {
        return result;
    }

    return SaveManager_initGhostsDir(this, dir);
}

static void SaveManager_init(SaveManager *this) {
    this->isValid = true;
    this->canSave = false;

    this->otherRawSave = this->rawSave;

    this->result = SaveManager_initGhosts(this);
    this->isBusy = false;
}

static void SaveManager_initTask(void *arg) {
    UNUSED(arg);

    SaveManager_init(s_saveManager);
}

static void my_SaveManager_initAsync(SaveManager *this) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, SaveManager_initTask, NULL, NULL);
}

PATCH_B(SaveManager_initAsync, my_SaveManager_initAsync);

static void my_SaveManager_resetAsync(SaveManager *this) {
    this->isValid = true;
    this->canSave = false;

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}

PATCH_B(SaveManager_resetAsync, my_SaveManager_resetAsync);

static void SaveManager_loadGhostHeaders(SaveManager *this) {
    for (u32 i = 0; i < this->ghostCount; i++) {
        GhostGroup_invalidate(this->ghostGroup, i);
        GhostGroup_readHeader(this->ghostGroup, i, this->rawGhostHeaders + i);
    }

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}

static void SaveManager_loadGhostHeadersTask(void *arg) {
    UNUSED(arg);

    SaveManager_loadGhostHeaders(s_saveManager);
}

static void my_SaveManager_loadGhostHeadersAsync(SaveManager *this, s32 licenseId, GhostGroup *group) {
    UNUSED(licenseId);

    if (group->count == 0) {
        return;
    }

    this->ghostGroup = group;

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, SaveManager_loadGhostHeadersTask, NULL, NULL);
}

PATCH_B(SaveManager_loadGhostHeadersAsync, my_SaveManager_loadGhostHeadersAsync);

static void SaveManager_loadGhosts(SaveManager *this) {
    RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    if (menuScenario->ghostBuffer == raceScenario->ghostBuffer) {
        if (menuScenario->ghostBuffer == s_raceConfig->ghostBuffers + 0) {
            menuScenario->ghostBuffer = s_raceConfig->ghostBuffers + 1;
        } else {
            menuScenario->ghostBuffer = s_raceConfig->ghostBuffers + 0;
        }
    }

    GlobalContext *cx = s_sectionManager->globalContext;
    for (u32 i = 0; i < cx->timeAttackGhostCount; i++) {
        u32 index = cx->timeAttackGhostIndices[i];
        const char *path = this->ghostPaths[index];
        this->result = NandHelper_readFile(path, this->rawGhostFile, 0x2800);
        if (this->result != RK_NAND_RESULT_OK) {
            break;
        }

        if (((RawGhostHeader *)this->rawGhostFile)->isCompressed) {
            RawGhostFile_decompress(this->rawGhostFile, (*menuScenario->ghostBuffer)[i]);
        } else {
            memcpy((*menuScenario->ghostBuffer)[i], this->rawGhostFile, 0x2800);
        }
    }

    this->isBusy = false;
}

static void SaveManager_loadGhostsTask(void *arg) {
    UNUSED(arg);

    SaveManager_loadGhosts(s_saveManager);
}

static void my_SaveManager_loadGhostAsync(SaveManager *this, s32 licenseId, u32 category, u32 index, u32 courseId) {
    UNUSED(licenseId);
    UNUSED(category);
    UNUSED(index);
    UNUSED(courseId);

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, SaveManager_loadGhostsTask, NULL, NULL);
}

PATCH_B(SaveManager_loadGhostAsync, my_SaveManager_loadGhostAsync);
