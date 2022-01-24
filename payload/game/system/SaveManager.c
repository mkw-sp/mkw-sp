#include "SaveManager.h"

#include "NandHelper.h"
#include "RaceConfig.h"
#include "ResourceManager.h"
#include "RootScene.h"
#include "Yaz.h"

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
    this->rawGhostHeaders = spAllocArray(MAX_GHOST_COUNT, sizeof(RawGhostHeader), 0x4, heap);
    this->ghostFooters = spAllocArray(MAX_GHOST_COUNT, sizeof(GhostFooter), 0x4, heap);
    this->ghostPaths = spAllocArray(MAX_GHOST_COUNT, NAND_MAX_PATH, 0x4, heap);
    this->courseSha1IsValid = spAllocArray(0x20, sizeof(bool), 0x4, heap);
    this->courseSha1s = spAllocArray(0x20, 5 * sizeof(u32), 0x4, heap);

    this->spCanSave = true;
    this->spBuffer = spAlloc(SP_BUFFER_SIZE, 0x20, heap);
    this->spSectionCount = 0;
    this->spSections = NULL;
    this->spLicenseCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->spLicenses); i++) {
        this->spLicenses[i] = NULL;
    }
    this->spCurrentLicense = -1;
    for (u32 i = 0; i < 0x20; i++) {
        this->courseSha1IsValid[i] = false;
    }

    return s_saveManager;
}
PATCH_B(SaveManager_createInstance, my_SaveManager_createInstance);

static void SaveManager_initGhost(SaveManager *this, const char *path) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    u32 length;
    if (NandHelper_readFile(path, this->rawGhostFile, 0x2800, &length) != RK_NAND_RESULT_OK) {
        return;
    }

    if (!RawGhostFile_spIsValid(this->rawGhostFile, length)) {
        return;
    }

    const RawGhostHeader *header = (RawGhostHeader *)this->rawGhostFile;
    memcpy(&this->rawGhostHeaders[this->ghostCount], header, sizeof(RawGhostHeader));
    GhostFooter_init(&this->ghostFooters[this->ghostCount], this->rawGhostFile, length);
    memcpy(&this->ghostPaths[this->ghostCount], path, NAND_MAX_PATH);
    this->ghostCount++;
}

static void SaveManager_initGhostsDir(SaveManager *this, const char *dir) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    if (strlen(dir) + 1 + NAND_MAX_NAME > NAND_MAX_PATH) {
        return;
    }

    u32 fileCount;

    if (NandHelper_readDir(dir, NULL, &fileCount) != RK_NAND_RESULT_OK) {
        return;
    }

    u32 size = OSRoundUp32B(fileCount * (NAND_MAX_NAME + 1));
    EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
    char *names = spAlloc(size, 0x20, heap);
    if (NandHelper_readDir(dir, names, &fileCount) != RK_NAND_RESULT_OK) {
        goto cleanup;
    }

    const char *name = names;
    for (u32 i = 0; i < fileCount; i++) {
        char path[NAND_MAX_PATH];
        snprintf(path, sizeof(path), "%s/%s", dir, name);

        u32 type;
        if (NandHelper_getType(path, &type) == RK_NAND_RESULT_OK) {
            switch (type) {
            case RK_NAND_TYPE_FILE:
                SaveManager_initGhost(this, path);
                break;
            case RK_NAND_TYPE_DIR:
                SaveManager_initGhostsDir(this, path);
                break;
            }
        }

        name += strlen(name) + 1;
    }

cleanup:
    spFree(names);
}

static void SaveManager_initGhosts(SaveManager *this) {
    char dir[NAND_MAX_PATH];
    if (NandHelper_getHomeDir(dir) != RK_NAND_RESULT_OK) {
        return;
    }

    u32 offset = strlen(dir);
    u32 size = strlen("/ghosts") + 1;
    if (offset + size > NAND_MAX_PATH) {
        return;
    }
    memcpy(dir + offset, "/ghosts", size);

    u8 perm = NAND_PERM_OWNER_READ | NAND_PERM_OWNER_WRITE;
    if (NandHelper_createDir(dir, perm) != RK_NAND_RESULT_OK) {
        return;
    }

    SaveManager_initGhostsDir(this, dir);
}

static bool setupSpSave(const char *path) {
    u8 perm = NAND_PERM_OWNER_READ | NAND_PERM_OWNER_WRITE;
    if (NandHelper_create(path, perm) != RK_NAND_RESULT_OK) {
        return false;
    }

    alignas(0x20) SpSaveHeader header = { .magic = SP_SAVE_HEADER_MAGIC, .crc32 = 0x0 };
    return NandHelper_writeFile(path, &header, sizeof(header)) == RK_NAND_RESULT_OK;
}

static bool SpSaveLicense_checkSize(const SpSaveLicense *this) {
    switch (this->version) {
        case SP_SAVE_LICENSE_VERSION:
            return this->size == sizeof(SpSaveLicense);
        default:
            return this->size >= sizeof(SpSaveLicense);
    }
}

static void SpSaveLicense_sanitize(SpSaveLicense *this) {
    switch (this->taRuleGhostTagVisibility) {
        case SP_TA_RULE_GHOST_TAG_VISIBILITY_NONE:
        case SP_TA_RULE_GHOST_TAG_VISIBILITY_WATCHED:
        case SP_TA_RULE_GHOST_TAG_VISIBILITY_ALL:
            break;
        default:
            this->taRuleGhostTagVisibility = SP_TA_RULE_GHOST_TAG_VISIBILITY_ALL;
    }
    switch (this->taRuleGhostTagContent) {
        case SP_TA_RULE_GHOST_TAG_CONTENT_NAME:
        case SP_TA_RULE_GHOST_TAG_CONTENT_TIME:
        case SP_TA_RULE_GHOST_TAG_CONTENT_DATE:
            break;
        default:
            this->taRuleGhostTagContent = SP_TA_RULE_GHOST_TAG_CONTENT_NAME;
    }
    switch (this->taRuleSolidGhosts) {
        case SP_TA_RULE_SOLID_GHOSTS_NONE:
        case SP_TA_RULE_SOLID_GHOSTS_WATCHED:
        case SP_TA_RULE_SOLID_GHOSTS_ALL:
            break;
        default:
            this->taRuleSolidGhosts = SP_TA_RULE_SOLID_GHOSTS_NONE;
    }
}

static bool SaveManager_initSpSave(SaveManager *this) {
    char path[NAND_MAX_PATH];
    if (NandHelper_getHomeDir(path) != RK_NAND_RESULT_OK) {
        return false;
    }

    u32 offset = strlen(path);
    u32 size = strlen("/save.bin") + 1;
    if (offset + size > NAND_MAX_PATH) {
        return false;
    }
    memcpy(path + offset, "/save.bin", size);

    u32 type;
    if (NandHelper_getType(path, &type) != RK_NAND_RESULT_OK) {
        return false;
    }
    switch (type) {
    case RK_NAND_TYPE_NONE:
        if (!setupSpSave(path)) {
            return false;
        }
        break;
    case RK_NAND_TYPE_FILE:
        break;
    case RK_NAND_TYPE_DIR:
        return false;
    }

    EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
    u32 length;
    if (NandHelper_readFile(path, this->spBuffer, SP_BUFFER_SIZE, &length) != RK_NAND_RESULT_OK) {
        return false;
    }

    offset = sizeof(SpSaveHeader);
    if (length < offset) {
        return false;
    }

    SpSaveHeader *header = this->spBuffer;
    u32 crc32 = NETCalcCRC32(this->spBuffer + offset, length - offset);
    if (crc32 != header->crc32) {
        return false;
    }

    u32 sectionCount;
    s32 unusedLicenseCount = MAX_SP_LICENSE_COUNT;
    for (sectionCount = 0; offset < length; sectionCount++) {
        if (length - offset < sizeof(SpSaveSection)) {
            return false;
        }

        SpSaveSection *section = this->spBuffer + offset;
        if (section->size < sizeof(SpSaveSection) || section->size > length - offset) {
            return false;
        }

        if (section->magic == SP_SAVE_LICENSE_MAGIC) {
            SpSaveLicense *license = (SpSaveLicense *)section;
            if (!SpSaveLicense_checkSize(license)) {
                return false;
            }
            SpSaveLicense_sanitize(license);
            unusedLicenseCount--;
        }

        offset += section->size;
    }

    if (unusedLicenseCount < 0) {
        unusedLicenseCount = 0;
    }

    u32 maxSectionCount = sectionCount + unusedLicenseCount;
    this->spSections = spAllocArray(maxSectionCount, sizeof(SpSaveSection *), 0x4, heap);
    offset = sizeof(SpSaveHeader);
    for (u32 i = 0; i < sectionCount; i++) {
        SpSaveSection *section = this->spBuffer + offset;
        this->spSections[i] = spAlloc(section->size, 0x4, heap);
        memcpy(this->spSections[i], section, section->size);
        offset += section->size;
    }
    this->spSectionCount = sectionCount;

    this->spLicenseCount = 0;
    for (u32 i = 0; i < sectionCount && this->spLicenseCount < MAX_SP_LICENSE_COUNT; i++) {
        if (this->spSections[i]->magic == SP_SAVE_LICENSE_MAGIC) {
            this->spLicenses[this->spLicenseCount++] = (SpSaveLicense *)this->spSections[i];
        }
    }

    for (u32 i = this->spLicenseCount; i < MAX_SP_LICENSE_COUNT; i++) {
        this->spLicenses[i] = spAlloc(sizeof(SpSaveLicense), 0x4, heap);
    }

    return true;
}

static void SaveManager_init(SaveManager *this) {
    this->isValid = true;
    this->canSave = false;

    this->otherRawSave = this->rawSave;

    if (!SaveManager_initSpSave(this)) {
        EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
        this->spSections = spAllocArray(MAX_SP_LICENSE_COUNT, sizeof(SpSaveSection *), 0x4, heap);
        for (u32 i = 0; i < MAX_SP_LICENSE_COUNT; i++) {
            this->spLicenses[i] = spAlloc(sizeof(SpSaveLicense), 0x4, heap);
        }
        this->result = RK_NAND_RESULT_NOSPACE;
        this->spCanSave = false;
    }

    SaveManager_initGhosts(this);

    this->isBusy = false;
}

static void initTask(void *arg) {
    UNUSED(arg);

    SaveManager_init(s_saveManager);
}

static void my_SaveManager_initAsync(SaveManager *this) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, initTask, NULL, NULL);
}
PATCH_B(SaveManager_initAsync, my_SaveManager_initAsync);

static void my_SaveManager_resetAsync(SaveManager *this) {
    this->isValid = true;
    this->canSave = false;

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}
PATCH_B(SaveManager_resetAsync, my_SaveManager_resetAsync);

static void SaveManager_saveSp(SaveManager *this) {
    u32 length = sizeof(SpSaveHeader);
    for (u32 i = 0; i < this->spSectionCount; i++) {
        if (length + this->spSections[i]->size > SP_BUFFER_SIZE) {
            goto fail;
        }
        memcpy(this->spBuffer + length, this->spSections[i], this->spSections[i]->size);
        length += this->spSections[i]->size;
    }

    char dir[NAND_MAX_PATH];
    if (NandHelper_getHomeDir(dir) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    u8 perm = NAND_PERM_OWNER_READ | NAND_PERM_OWNER_WRITE;
    if (NandHelper_create("/tmp/save.bin", perm) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    u32 crc32 = NETCalcCRC32(this->spBuffer + sizeof(SpSaveHeader), length - sizeof(SpSaveHeader));
    SpSaveHeader header = { .magic = SP_SAVE_HEADER_MAGIC, .crc32 = crc32 };
    memcpy(this->spBuffer, &header, sizeof(header));
    if (NandHelper_writeFile("/tmp/save.bin", this->spBuffer, length) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    if (NandHelper_move("/tmp/save.bin", dir) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
    return;

fail:
    this->spCanSave = false;
    this->isBusy = false;
    this->result = RK_NAND_RESULT_NOSPACE;
}

static void saveSpTask(void *arg) {
    UNUSED(arg);

    SaveManager_saveSp(s_saveManager);
}

static void my_SaveManager_saveLicensesAsync(SaveManager *this) {
    if (!this->spCanSave) {
        this->isBusy = false;
        this->result = RK_NAND_RESULT_OK;
    }

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, saveSpTask, NULL, NULL);
}
PATCH_B(SaveManager_saveLicensesAsync, my_SaveManager_saveLicensesAsync);

void SaveManager_eraseSpLicense(SaveManager *this) {
    const SpSaveLicense *license = this->spLicenses[this->spCurrentLicense];
    for (u32 i = this->spCurrentLicense; i < this->spLicenseCount - 1; i++) {
        this->spLicenses[i] = this->spLicenses[i + 1];
    }
    this->spLicenseCount--;
    for (u32 i = 0; i < this->spSectionCount; i++) {
        if (this->spSections[i] == (SpSaveSection *)license) {
            for (u32 j = i; j < this->spSectionCount - 1; j++) {
                this->spSections[j] = this->spSections[j + 1];
            }
            break;
        }
    }
    this->spSectionCount--;
    this->spCurrentLicense = -1;
}

void SaveManager_createSpLicense(SaveManager *this, const MiiId *miiId) {
    SpSaveLicense *license = this->spLicenses[this->spLicenseCount++];
    license->magic = SP_SAVE_LICENSE_MAGIC;
    license->size = sizeof(SpSaveLicense);
    license->version = SP_SAVE_LICENSE_VERSION;
    license->miiId = *miiId;
    license->driftMode = SP_DRIFT_MODE_MANUAL;
    license->settingHudLabels = SP_SETTING_HUD_LABELS_SHOW;
    license->setting169Fov = SP_SETTING_169_FOV_DEFAULT;
    license->settingMapIcons = SP_SETTING_MAP_ICONS_MIIS;
    license->taRuleClass = SP_TA_RULE_CLASS_150CC;
    license->taRuleGhostSorting = SP_TA_RULE_GHOST_SORTING_FASTEST;
    license->taRuleGhostTagVisibility = SP_TA_RULE_GHOST_TAG_VISIBILITY_ALL;
    license->taRuleGhostTagContent = SP_TA_RULE_GHOST_TAG_CONTENT_NAME;
    license->taRuleSolidGhosts = SP_TA_RULE_SOLID_GHOSTS_NONE;
    this->spSections[this->spSectionCount++] = license;
    this->spCurrentLicense = this->spLicenseCount - 1;
}

void SaveManager_changeSpLicenseMiiId(const SaveManager *this, const MiiId *miiId) {
    if (this->spCurrentLicense < 0) {
        return;
    }

    this->spLicenses[this->spCurrentLicense]->miiId = *miiId;
}

u32 SaveManager_getSettingHudLabels(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->settingHudLabels;
}

u32 SaveManager_getSetting169Fov(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->setting169Fov;
}

u32 SaveManager_getSettingMapIcons(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->settingMapIcons;
}

u32 SaveManager_getTaRuleClass(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->taRuleClass;
}

u32 SaveManager_getTaRuleGhostSorting(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->taRuleGhostSorting;
}

u32 SaveManager_getTaRuleGhostTagVisibility(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->taRuleGhostTagVisibility;
}

u32 SaveManager_getTaRuleGhostTagContent(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->taRuleGhostTagContent;
}

u32 SaveManager_getTaRuleSolidGhosts(const SaveManager *this) {
    return this->spLicenses[this->spCurrentLicense]->taRuleSolidGhosts;
}

static void SaveManager_loadGhostHeaders(SaveManager *this) {
    for (u32 i = 0; i < this->ghostCount; i++) {
        GhostGroup_invalidate(this->ghostGroup, i);
        GhostGroup_readHeader(this->ghostGroup, i, this->rawGhostHeaders + i);
    }

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}

static void loadGhostHeadersTask(void *arg) {
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
    EGG_TaskThread_request(this->taskThread, loadGhostHeadersTask, NULL, NULL);
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
        u32 length;
        this->result = NandHelper_readFile(path, this->rawGhostFile, 0x2800, &length);
        if (this->result != RK_NAND_RESULT_OK) {
            memset((*menuScenario->ghostBuffer)[i], 0, 0x2800);
            continue;
        }

        if (((RawGhostHeader *)this->rawGhostFile)->isCompressed) {
            if (!RawGhostFile_spIsValid(this->rawGhostFile, length)) {
                memset((*menuScenario->ghostBuffer)[i], 0, 0x2800);
                continue;
            }

            if (!RawGhostFile_spDecompress(this->rawGhostFile, (*menuScenario->ghostBuffer)[i])) {
                memset((*menuScenario->ghostBuffer)[i], 0, 0x2800);
                continue;
            }
        } else {
            memcpy((*menuScenario->ghostBuffer)[i], this->rawGhostFile, 0x2800);
        }

        if (!RawGhostFile_spIsValid((*menuScenario->ghostBuffer)[i], 0x2800)) {
            memset((*menuScenario->ghostBuffer)[i], 0, 0x2800);
        }
    }

    this->isBusy = false;
}

static void loadGhostsTask(void *arg) {
    UNUSED(arg);

    SaveManager_loadGhosts(s_saveManager);
}

static void my_SaveManager_loadGhostAsync(SaveManager *this, s32 licenseId, u32 category, u32 index, u32 courseId) {
    UNUSED(licenseId);
    UNUSED(category);
    UNUSED(index);
    UNUSED(courseId);

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, loadGhostsTask, NULL, NULL);
}
PATCH_B(SaveManager_loadGhostAsync, my_SaveManager_loadGhostAsync);

static char nibbleToChar(u8 val) {
    if (val < 0xa) {
        return '0' + val;
    }
    return 'a' + val - 0xa;
}

static void SaveManager_saveGhost(SaveManager *this, GhostFile *file) {
    this->saveGhostResult = false;

    if (file->raceTime.minutes > 99) {
        goto fail;
    }

    u32 length = GhostFile_spWrite(file, this->rawGhostFile);
    if (length == 0) {
        goto fail;
    }

    char dir[NAND_MAX_PATH];
    if (NandHelper_getHomeDir(dir) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    u32 offset = strlen(dir);
    u32 dirSize = strlen("/ghosts/") + 1;
    if (offset + dirSize > NAND_MAX_PATH) {
        goto fail;
    }
    memcpy(dir + offset, "/ghosts/", dirSize);

    offset += dirSize - 1;
    if (offset + NAND_MAX_NAME + 1 > NAND_MAX_PATH) {
        goto fail;
    }
    const u8 *courseSha1 = (const u8 *)this->courseSha1s[file->courseId];
    for (u32 i = 0; i < NAND_MAX_NAME; i += 2) {
        dir[offset + i] = nibbleToChar(courseSha1[i / 2] >> 4);
        dir[offset + i + 1] = nibbleToChar(courseSha1[i / 2] & 0xf);
    }
    dir[offset + NAND_MAX_NAME] = '\0';

    u8 perm = NAND_PERM_OWNER_READ | NAND_PERM_OWNER_WRITE;
    if (NandHelper_createDir(dir, perm) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    u16 mins = file->raceTime.minutes;
    u8 secs = file->raceTime.seconds;
    u16 msecs = file->raceTime.milliseconds;
    char path[NAND_MAX_PATH];
    snprintf(path, NAND_MAX_PATH, "%s/%02u%02u%03u.rkg", dir, mins, secs, msecs);
    u32 type;
    if (NandHelper_getType(path, &type) != RK_NAND_RESULT_OK) {
        goto fail;
    }
    if (type != RK_NAND_TYPE_NONE) {
        char c;
        for (c = 'a'; c <= 'z'; c++) {
            snprintf(path, NAND_MAX_PATH, "%s/%02u%02u%03u%c.rkg", dir, mins, secs, msecs, c);
            if (NandHelper_getType(path, &type) != RK_NAND_RESULT_OK) {
                goto fail;
            }
            if (type == RK_NAND_TYPE_NONE) {
                break;
            }
        }
        if (c > 'z') {
            goto fail;
        }
    }

    if (NandHelper_create(path, perm) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    if (NandHelper_writeFile(path, this->rawGhostFile, length) != RK_NAND_RESULT_OK) {
        goto fail;
    }

    this->saveGhostResult = true;

    if (RawGhostFile_spIsValid(this->rawGhostFile, length) && this->ghostCount < MAX_GHOST_COUNT) {
        const RawGhostHeader *header = (RawGhostHeader *)this->rawGhostFile;
        memcpy(&this->rawGhostHeaders[this->ghostCount], header, sizeof(RawGhostHeader));
        GhostFooter_init(&this->ghostFooters[this->ghostCount], this->rawGhostFile, length);
        memcpy(&this->ghostPaths[this->ghostCount], path, NAND_MAX_PATH);
        this->ghostCount++;
    }

fail:
    this->isBusy = false;
}

static void saveGhostTask(void *arg) {
    GhostFile *file = arg;
    SaveManager_saveGhost(s_saveManager, file);
}

static void my_SaveManager_saveGhostAsync(SaveManager *this, s32 licenseId, u32 category, u32 index, GhostFile *file, bool saveLicense) {
    UNUSED(licenseId);
    UNUSED(category);
    UNUSED(index);
    UNUSED(saveLicense);

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, saveGhostTask, file, NULL);
}
PATCH_B(SaveManager_saveGhostAsync, my_SaveManager_saveGhostAsync);

static void SaveManager_computeCourseSha1(SaveManager *this, u32 courseId) {
    char path[0x40];
    snprintf(path, sizeof(path), "Race/Course/%s.szs", courseFilenames[courseId]);
    DVDFileInfo fileInfo;
    if (!DVDOpen(path, &fileInfo)) {
        return;
    }

    u32 fileSize = OSRoundUp32B(fileInfo.length), fileOffset = 0x0;
    YazDecoder yazDecoder;
    YazDecoder_init(&yazDecoder);
    u32 yazResult = YAZ_DECODER_RESULT_WANTS_SRC;
    NETSHA1Context sha1Cx;
    NETSHA1Init(&sha1Cx);
    alignas(0x20) u8 srcBuffer[0x1000];
    alignas(0x20) u8 dstBuffer[0x1000];
    while (fileOffset < fileSize && yazResult != YAZ_DECODER_RESULT_DONE) {
        u32 srcSize = fileSize - fileOffset;
        if (srcSize > sizeof(srcBuffer)) {
            srcSize = sizeof(srcBuffer);
        }
        if (DVDRead(&fileInfo, srcBuffer, srcSize, fileOffset) != (s32)srcSize) {
            goto cleanup;
        }
        fileOffset += srcSize;

        const u8 *src = srcBuffer;
        do {
            u8 *dst = dstBuffer;
            u32 dstSize = sizeof(dstBuffer);
            yazResult = YazDecoder_feed(&yazDecoder, &src, &dst, &srcSize, &dstSize);
            if (yazResult == YAZ_DECODER_RESULT_ERROR) {
                goto cleanup;
            }

            NETSHA1Update(&sha1Cx, dstBuffer, dst - dstBuffer);
        } while (yazResult == YAZ_DECODER_RESULT_WANTS_DST);
    }

    NETSHA1GetDigest(&sha1Cx, this->courseSha1s[courseId]);
    this->courseSha1IsValid[courseId] = true;

    this->isBusy = false;

cleanup:
    DVDClose(&fileInfo);
}

static void computeCourseSha1Task(void *arg) {
    u32 courseId = (u32)arg;
    SaveManager_computeCourseSha1(s_saveManager, courseId);
    while (s_saveManager->isBusy) {
        VIWaitForRetrace();
        SaveManager_computeCourseSha1(s_saveManager, courseId);
    }
}

bool SaveManager_computeCourseSha1Async(SaveManager *this, u32 courseId) {
    if (this->courseSha1IsValid[courseId]) {
        return true;
    }

    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, computeCourseSha1Task, (void *)courseId, NULL);
    return false;
}

const u32 *SaveManager_getCourseSha1(const SaveManager *this, u32 courseId) {
    return this->courseSha1s[courseId];
}

bool vsSpeedModIsEnabled;
