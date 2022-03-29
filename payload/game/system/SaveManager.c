#include "SaveManager.h"

#include "NandHelper.h"
#include "RaceConfig.h"
#include "ResourceManager.h"
#include "RootScene.h"
#include <sp/Yaz.h>

#include "../ui/SectionManager.h"

#include <stdalign.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

enum {
    GHOST_INIT_STACK_SIZE = 0x8000, // 32 KiB
};

SaveManager *SaveManager_ct(SaveManager *this);

SaveManager *my_SaveManager_createInstance(void) {
    s_saveManager = new(sizeof(SaveManager));
    SaveManager_ct(s_saveManager);
    SaveManager *this = s_saveManager;

    this->ghostCount = 0;
    EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
    this->rawGhostHeaders = spAllocArray(MAX_GHOST_COUNT, sizeof(RawGhostHeader), 0x4, heap);
    this->ghostFooters = spAllocArray(MAX_GHOST_COUNT, sizeof(GhostFooter), 0x4, heap);
    this->ghostIds = spAllocArray(MAX_GHOST_COUNT, sizeof(NodeId), 0x4, heap);
    this->ghostInitStack = spAlloc(GHOST_INIT_STACK_SIZE, 0x4, heap);
    this->courseSha1IsValid = spAllocArray(0x20, sizeof(bool), 0x4, heap);
    for (u32 i = 0; i < 0x20; i++) {
        this->courseSha1IsValid[i] = false;
    }
    this->courseSha1s = spAllocArray(0x20, 0x14, 0x4, heap);

    this->spCanSave = true;
    this->spBuffer = spAlloc(SP_BUFFER_SIZE, 0x20, heap);
    this->spSectionCount = 0;
    this->spSections = NULL;
    this->spLicenseCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->spLicenses); i++) {
        this->spLicenses[i] = NULL;
    }
    this->spCurrentLicense = -1;

    return s_saveManager;
}
PATCH_B(SaveManager_createInstance, my_SaveManager_createInstance);

static void SaveManager_initGhost(SaveManager *this, const NodeId id) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    u32 readSize;
    if (!Storage_fastReadFile(id, this->rawGhostFile, 0x2800, &readSize)) {
        return;
    }

    if (!RawGhostFile_spIsValid(this->rawGhostFile, readSize)) {
        return;
    }

    const RawGhostHeader *header = (RawGhostHeader *)this->rawGhostFile;
    memcpy(&this->rawGhostHeaders[this->ghostCount], header, sizeof(RawGhostHeader));
    GhostFooter_init(&this->ghostFooters[this->ghostCount], this->rawGhostFile, readSize);
    this->ghostIds[this->ghostCount] = id;
    this->ghostCount++;
}

static void SaveManager_initGhostsDir(SaveManager *this, wchar_t *path, u32 offset) {
    if (this->ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    Dir dir;
    if (!Storage_openDir(&dir, path)) {
        return;
    }

    NodeInfo info;
    while (Storage_readDir(&dir, &info)) {
        if (info.type == NODE_TYPE_NONE) {
            break;
        }
        s32 length = swprintf(path + offset, 2048 - offset, L"/%ls", info.name);
        if (length < 0) {
            continue;
        }
        offset += length;
        if (info.type == NODE_TYPE_FILE) {
            SaveManager_initGhost(this, info.id);
        } else if (info.type == NODE_TYPE_DIR) {
            SaveManager_initGhostsDir(this, path, offset);
        } else {
            break;
        }
        offset -= length;
    }

    Storage_closeDir(&dir);
}

static void SaveManager_initGhosts(SaveManager *this) {
    SP_LOG("Initializing ghosts...");

    wchar_t path[2048];
    u32 offset = swprintf(path, 2048, L"/mkw-sp/ghosts");
    SaveManager_initGhostsDir(this, path, offset);
    offset = swprintf(path, 2048, L"/ctgpr/ghosts");
    SaveManager_initGhostsDir(this, path, offset);

    Storage_createDir(L"/mkw-sp/ghosts", true);

    SP_LOG("Ghosts: %u / %u", this->ghostCount, MAX_GHOST_COUNT);
}

static void *SaveManager_initGhostsTask(void *UNUSED(arg)) {
    SaveManager_initGhosts(s_saveManager);
    return NULL;
}

static void SaveManager_initGhostsAsync(SaveManager *this) {
    void *stackBase = this->ghostInitStack + GHOST_INIT_STACK_SIZE;
    OSCreateThread(&this->ghostInitThread, SaveManager_initGhostsTask, NULL, stackBase,
            GHOST_INIT_STACK_SIZE, 24, 0);
    OSResumeThread(&this->ghostInitThread);
}

static bool SpSaveLicense_checkSize(const SpSaveLicense *this) {
    switch (this->version) {
    case 0:
    case 1:
    case 2:
        return this->size == 0x18;
    case SP_SAVE_LICENSE_VERSION:
        return this->size == sizeof(SpSaveLicense);
    default:
        return this->size >= sizeof(SpSaveLicense);
    }
}

static void SpSaveLicense_upgrade(SpSaveLicense *this) {
    if (this->version < 1) {
        this->settingPageTransitions = SP_SETTING_PAGE_TRANSITIONS_DEFAULT;
    }

    if (this->version < 2) {
        this->settingRaceInputDisplay = SP_SETTING_RACE_INPUT_DISPLAY_DEFAULT;
    }

    if (this->version < 3) {
        this->taRuleGhostSound = SP_TA_RULE_GHOST_SOUND_DEFAULT;
    }

    if (this->version < SP_SAVE_LICENSE_VERSION) {
        this->size = sizeof(SpSaveLicense);
        this->version = SP_SAVE_LICENSE_VERSION;
    }
}

static void SpSaveLicense_sanitize(SpSaveLicense *this) {
    switch (this->settingRaceInputDisplay) {
    case SP_SETTING_RACE_INPUT_DISPLAY_DISABLE:
    case SP_SETTING_RACE_INPUT_DISPLAY_SIMPLE:
        break;
    default:
        this->settingRaceInputDisplay = SP_SETTING_RACE_INPUT_DISPLAY_DEFAULT;
    }
    switch (this->taRuleGhostTagVisibility) {
    case SP_TA_RULE_GHOST_TAG_VISIBILITY_NONE:
    case SP_TA_RULE_GHOST_TAG_VISIBILITY_WATCHED:
    case SP_TA_RULE_GHOST_TAG_VISIBILITY_ALL:
        break;
    default:
        this->taRuleGhostTagVisibility = SP_TA_RULE_GHOST_TAG_VISIBILITY_DEFAULT;
    }
    switch (this->taRuleSolidGhosts) {
    case SP_TA_RULE_SOLID_GHOSTS_NONE:
    case SP_TA_RULE_SOLID_GHOSTS_WATCHED:
    case SP_TA_RULE_SOLID_GHOSTS_ALL:
        break;
    default:
        this->taRuleSolidGhosts = SP_TA_RULE_SOLID_GHOSTS_DEFAULT;
    }
    switch (this->taRuleGhostSound) {
    case SP_TA_RULE_GHOST_SOUND_NONE:
    case SP_TA_RULE_GHOST_SOUND_WATCHED:
    case SP_TA_RULE_GHOST_SOUND_ALL:
        break;
    default:
        this->taRuleGhostSound = SP_TA_RULE_GHOST_SOUND_DEFAULT;
    }
}

static bool SaveManager_initSpSave(SaveManager *this) {
    const wchar_t path[] = L"/mkw-sp/save.bin";
    NodeInfo info;
    Storage_stat(path, &info);
    switch (info.type) {
    case NODE_TYPE_NONE:;
        alignas(0x20) SpSaveHeader header = { .magic = SP_SAVE_HEADER_MAGIC, .crc32 = 0x0 };
        if (!Storage_writeFile(path, true, &header, sizeof(header))) {
            return false;
        }
        break;
    case NODE_TYPE_FILE:
        break;
    default:
        return false;
    }

    EGG_Heap *heap = s_rootScene->heapCollection.heaps[HEAP_ID_MEM2];
    u32 size;
    if (!Storage_readFile(path, this->spBuffer, SP_BUFFER_SIZE, &size)) {
        return false;
    }

    u32 offset = sizeof(SpSaveHeader);
    if (size < offset) {
        return false;
    }

    SpSaveHeader *header = this->spBuffer;
    u32 crc32 = NETCalcCRC32(this->spBuffer + offset, size - offset);
    if (crc32 != header->crc32) {
        return false;
    }

    u32 sectionCount;
    s32 unusedLicenseCount = MAX_SP_LICENSE_COUNT;
    for (sectionCount = 0; offset < size; sectionCount++) {
        if (size - offset < sizeof(SpSaveSection)) {
            return false;
        }

        SpSaveSection *section = this->spBuffer + offset;
        if (section->size < sizeof(SpSaveSection) || section->size > size - offset) {
            return false;
        }

        if (section->magic == SP_SAVE_LICENSE_MAGIC) {
            const SpSaveLicense *license = (SpSaveLicense *)section;
            if (!SpSaveLicense_checkSize(license)) {
                return false;
            }
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
        const SpSaveSection *section = this->spBuffer + offset;
        this->spSections[i] = spAlloc(section->size, 0x4, heap);
        memcpy(this->spSections[i], section, section->size);
        if (section->magic == SP_SAVE_LICENSE_MAGIC) {
            SpSaveLicense *license = (SpSaveLicense *)this->spSections[i];
            SpSaveLicense_upgrade(license);
            SpSaveLicense_sanitize(license);
        }
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

    SaveManager_initGhostsAsync(this);

    this->isBusy = false;
}

static void initTask(void *UNUSED(arg)) {
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
    u32 size = sizeof(SpSaveHeader);
    for (u32 i = 0; i < this->spSectionCount; i++) {
        if (size + this->spSections[i]->size > SP_BUFFER_SIZE) {
            goto fail;
        }
        memcpy(this->spBuffer + size, this->spSections[i], this->spSections[i]->size);
        size += this->spSections[i]->size;
    }

    u32 crc32 = NETCalcCRC32(this->spBuffer + sizeof(SpSaveHeader), size - sizeof(SpSaveHeader));
    SpSaveHeader header = { .magic = SP_SAVE_HEADER_MAGIC, .crc32 = crc32 };
    memcpy(this->spBuffer, &header, sizeof(header));

    if (!Storage_writeFile(L"/mkw-sp/save.bin.new", true, this->spBuffer, size)) {
        goto fail;
    }

    if (!Storage_remove(L"/mkw-sp/save.bin.old", true)) {
        goto fail;
    }

    if (!Storage_rename(L"/mkw-sp/save.bin", L"/mkw-sp/save.bin.old")) {
        goto fail;
    }

    if (!Storage_rename(L"/mkw-sp/save.bin.new", L"/mkw-sp/save.bin")) {
        goto fail;
    }

    Storage_remove(L"/mkw-sp/save.bin.old", true); // Not a big deal if this fails

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
    return;

fail:
    this->spCanSave = false;
    this->isBusy = false;
    this->result = RK_NAND_RESULT_NOSPACE;
}

static void saveSpTask(void *UNUSED(arg)) {
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
    license->driftMode = SP_DRIFT_MODE_DEFAULT;
    license->settingHudLabels = SP_SETTING_HUD_LABELS_DEFAULT;
    license->setting169Fov = SP_SETTING_169_FOV_DEFAULT;
    license->settingMapIcons = SP_SETTING_MAP_ICONS_DEFAULT;
    license->settingPageTransitions = SP_SETTING_PAGE_TRANSITIONS_DEFAULT;
    license->settingRaceInputDisplay = SP_SETTING_RACE_INPUT_DISPLAY_DEFAULT;
    license->taRuleClass = SP_TA_RULE_CLASS_DEFAULT;
    license->taRuleGhostSorting = SP_TA_RULE_GHOST_SORTING_DEFAULT;
    license->taRuleGhostTagVisibility = SP_TA_RULE_GHOST_TAG_VISIBILITY_DEFAULT;
    license->taRuleGhostTagContent = SP_TA_RULE_GHOST_TAG_CONTENT_DEFAULT;
    license->taRuleSolidGhosts = SP_TA_RULE_SOLID_GHOSTS_DEFAULT;
    license->taRuleGhostSound = SP_TA_RULE_GHOST_SOUND_DEFAULT;
    this->spSections[this->spSectionCount++] = license;
    this->spCurrentLicense = this->spLicenseCount - 1;
}

void SaveManager_changeSpLicenseMiiId(const SaveManager *this, const MiiId *miiId) {
    if (this->spCurrentLicense < 0) {
        return;
    }

    this->spLicenses[this->spCurrentLicense]->miiId = *miiId;
}

u32 SaveManager_getDriftMode(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_DRIFT_MODE_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->driftMode;
}

void SaveManager_setDriftMode(SaveManager *this, u32 driftMode) {
    if (this->spCurrentLicense < 0) {
        return;
    }

    this->spLicenses[this->spCurrentLicense]->driftMode = driftMode;
}

u32 SaveManager_getSettingHudLabels(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_SETTING_HUD_LABELS_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->settingHudLabels;
}

u32 SaveManager_getSetting169Fov(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_SETTING_HUD_LABELS_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->setting169Fov;
}

u32 SaveManager_getSettingMapIcons(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_SETTING_MAP_ICONS_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->settingMapIcons;
}

u32 SaveManager_getSettingPageTransitions(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_SETTING_PAGE_TRANSITIONS_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->settingPageTransitions;
}

void SaveManager_setSettingPageTransitions(const SaveManager *this, u32 pageTransitions) {
    if (this->spCurrentLicense < 0) {
        return;
    }

    this->spLicenses[this->spCurrentLicense]->settingPageTransitions = pageTransitions;
}

u32 SaveManager_getSettingRaceInputDisplay(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_SETTING_RACE_INPUT_DISPLAY_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->settingRaceInputDisplay;
}

void SaveManager_setSettingRaceInputDisplay(SaveManager *this, u32 raceInputDisplay) {
    if (this->spCurrentLicense < 0) {
        return;
    }

    this->spLicenses[this->spCurrentLicense]->settingRaceInputDisplay = raceInputDisplay;
}

u32 SaveManager_getTaRuleClass(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_TA_RULE_CLASS_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->taRuleClass;
}

u32 SaveManager_getTaRuleGhostSorting(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_TA_RULE_GHOST_SORTING_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->taRuleGhostSorting;
}

u32 SaveManager_getTaRuleGhostTagVisibility(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_TA_RULE_GHOST_TAG_VISIBILITY_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->taRuleGhostTagVisibility;
}

u32 SaveManager_getTaRuleGhostTagContent(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_TA_RULE_GHOST_TAG_CONTENT_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->taRuleGhostTagContent;
}

u32 SaveManager_getTaRuleSolidGhosts(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_TA_RULE_SOLID_GHOSTS_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->taRuleSolidGhosts;
}

u32 SaveManager_getTaRuleGhostSound(const SaveManager *this) {
    if (this->spCurrentLicense < 0) {
        return SP_TA_RULE_GHOST_SOUND_DEFAULT;
    }

    return this->spLicenses[this->spCurrentLicense]->taRuleGhostSound;
}

static void SaveManager_loadGhostHeaders(SaveManager *this) {
    OSJoinThread(&this->ghostInitThread, NULL);
    OSDetachThread(&this->ghostInitThread);

    this->isBusy = false;
    this->result = RK_NAND_RESULT_OK;
}

static void loadGhostHeadersTask(void *UNUSED(arg)) {
    SaveManager_loadGhostHeaders(s_saveManager);
}

static void my_SaveManager_loadGhostHeadersAsync(SaveManager *this, s32 UNUSED(licenseId),
        GhostGroup *UNUSED(group)) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, loadGhostHeadersTask, NULL, NULL);
}
PATCH_B(SaveManager_loadGhostHeadersAsync, my_SaveManager_loadGhostHeadersAsync);

static bool SaveManager_loadGhost(SaveManager *this, u32 i) {
    const GlobalContext *cx = s_sectionManager->globalContext;
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;

    NodeId id = this->ghostIds[cx->timeAttackGhostIndices[i]];
    u32 readSize;
    if (!Storage_fastReadFile(id, this->rawGhostFile, 0x2800, &readSize)) {
        return false;
    }

    if (((RawGhostHeader *)this->rawGhostFile)->isCompressed) {
        if (!RawGhostFile_spIsValid(this->rawGhostFile, readSize)) {
            return false;
        }

        if (!RawGhostFile_spDecompress(this->rawGhostFile, (*menuScenario->ghostBuffer)[i])) {
            return false;
        }
    } else {
        memcpy((*menuScenario->ghostBuffer)[i], this->rawGhostFile, 0x2800);
    }

    return RawGhostFile_spIsValid((*menuScenario->ghostBuffer)[i], 0x2800);
}

static void SaveManager_loadGhosts(SaveManager *this) {
    OSJoinThread(&this->ghostInitThread, NULL);
    OSDetachThread(&this->ghostInitThread);

    RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    if (menuScenario->ghostBuffer == raceScenario->ghostBuffer) {
        if (menuScenario->ghostBuffer == s_raceConfig->ghostBuffers + 0) {
            menuScenario->ghostBuffer = s_raceConfig->ghostBuffers + 1;
        } else {
            menuScenario->ghostBuffer = s_raceConfig->ghostBuffers + 0;
        }
    }

    const GlobalContext *cx = s_sectionManager->globalContext;
    for (u32 i = 0; i < cx->timeAttackGhostCount; i++) {
        if (!SaveManager_loadGhost(this, i)) {
            memset((*menuScenario->ghostBuffer)[i], 0, 0x2800);
        }
    }

    this->isBusy = false;
}

static void loadGhostsTask(void *UNUSED(arg)) {
    SaveManager_loadGhosts(s_saveManager);
}

static void my_SaveManager_loadGhostAsync(SaveManager *this, s32 UNUSED(licenseId),
        u32 UNUSED(category), u32 UNUSED(index), u32 UNUSED(courseId)) {
    this->isBusy = true;
    EGG_TaskThread_request(this->taskThread, loadGhostsTask, NULL, NULL);
}
PATCH_B(SaveManager_loadGhostAsync, my_SaveManager_loadGhostAsync);

static const u8 courseSha1s[0x20][0x14] = {
    { 0x1a, 0xe1, 0xa7, 0xd8, 0x94, 0x96, 0x0b, 0x38, 0xe0, 0x9e, 0x74, 0x94, 0x37, 0x33, 0x78,
      0xd8, 0x73, 0x05, 0xa1, 0x63 },
    { 0x90, 0x72, 0x0a, 0x7d, 0x57, 0xa7, 0xc7, 0x6e, 0x23, 0x47, 0x78, 0x2f, 0x6b, 0xde, 0x5d,
      0x22, 0x34, 0x2f, 0xb7, 0xdd },
    { 0x0e, 0x38, 0x03, 0x57, 0xaf, 0xfc, 0xfd, 0x87, 0x22, 0x32, 0x99, 0x94, 0x88, 0x56, 0x99,
      0xd9, 0x92, 0x7f, 0x82, 0x76 },
    { 0x18, 0x96, 0xae, 0xa4, 0x96, 0x17, 0xa5, 0x71, 0xc6, 0x6f, 0xf7, 0x78, 0xd8, 0xf2, 0xab,
      0xbe, 0x9e, 0x5d, 0x74, 0x79 },
    { 0x77, 0x52, 0xbb, 0x51, 0xed, 0xbc, 0x4a, 0x95, 0x37, 0x7c, 0x0a, 0x05, 0xb0, 0xe0, 0xda,
      0x15, 0x03, 0x78, 0x66, 0x25 },
    { 0xe4, 0xbf, 0x36, 0x4c, 0xb0, 0xc5, 0x89, 0x99, 0x07, 0x58, 0x5d, 0x73, 0x16, 0x21, 0xca,
      0x93, 0x0a, 0x4e, 0xf8, 0x5c },
    { 0xb0, 0x2e, 0xd7, 0x2e, 0x00, 0xb4, 0x00, 0x64, 0x7b, 0xda, 0x68, 0x45, 0xbe, 0x38, 0x7c,
      0x47, 0xd2, 0x51, 0xf9, 0xd1 },
    { 0xd1, 0xa4, 0x53, 0xb4, 0x3d, 0x69, 0x20, 0xa7, 0x85, 0x65, 0xe6, 0x5a, 0x45, 0x97, 0xe3,
      0x53, 0xb1, 0x77, 0xab, 0xd0 },
    { 0x72, 0xd0, 0x24, 0x1c, 0x75, 0xbe, 0x4a, 0x5e, 0xbd, 0x24, 0x2b, 0x9d, 0x8d, 0x89, 0xb1,
      0xd6, 0xfd, 0x56, 0xbe, 0x8f },
    { 0x52, 0xf0, 0x1a, 0xe3, 0xae, 0xd1, 0xe0, 0xfa, 0x4c, 0x74, 0x59, 0xa6, 0x48, 0x49, 0x48,
      0x63, 0xe8, 0x3a, 0x54, 0x8c },
    { 0x48, 0xeb, 0xd9, 0xd6, 0x44, 0x13, 0xc2, 0xb9, 0x8d, 0x2b, 0x92, 0xe5, 0xef, 0xc9, 0xb1,
      0x5e, 0xcd, 0x76, 0xfe, 0xe6 },
    { 0xac, 0xc0, 0x88, 0x3a, 0xe0, 0xce, 0x78, 0x79, 0xc6, 0xef, 0xba, 0x20, 0xcf, 0xe5, 0xb5,
      0x90, 0x9b, 0xf7, 0x84, 0x1b },
    { 0x38, 0x48, 0x6c, 0x4f, 0x70, 0x63, 0x95, 0x77, 0x2b, 0xd9, 0x88, 0xc1, 0xac, 0x5f, 0xa3,
      0x0d, 0x27, 0xca, 0xe0, 0x98 },
    { 0xb1, 0x3c, 0x51, 0x54, 0x75, 0xd7, 0xda, 0x20, 0x7d, 0xfd, 0x5b, 0xad, 0xd8, 0x86, 0x98,
      0x61, 0x47, 0xb9, 0x06, 0xff },
    { 0xb9, 0x82, 0x1b, 0x14, 0xa8, 0x93, 0x81, 0xf9, 0xc0, 0x15, 0x66, 0x93, 0x53, 0xcb, 0x24,
      0xd7, 0xdb, 0x1b, 0xb2, 0x5d },
    { 0xff, 0xe5, 0x18, 0x91, 0x5e, 0x5f, 0xaa, 0xa8, 0x89, 0x05, 0x7c, 0x8a, 0x3d, 0x3e, 0x43,
      0x98, 0x68, 0x57, 0x45, 0x08 },
    { 0x80, 0x14, 0x48, 0x8a, 0x60, 0xf4, 0x42, 0x8e, 0xef, 0x52, 0xd0, 0x1f, 0x8c, 0x58, 0x61,
      0xca, 0x95, 0x65, 0xe1, 0xca },
    { 0x8c, 0x85, 0x4b, 0x08, 0x74, 0x17, 0xa9, 0x24, 0x25, 0x11, 0x0c, 0xc7, 0x1e, 0x23, 0xc9,
      0x44, 0xd6, 0x99, 0x78, 0x06 },
    { 0x07, 0x1d, 0x69, 0x7c, 0x4d, 0xdb, 0x66, 0xd3, 0xb2, 0x10, 0xf3, 0x6c, 0x7b, 0xf8, 0x78,
      0x50, 0x2e, 0x79, 0x84, 0x5b },
    { 0x49, 0x51, 0x4e, 0x8f, 0x74, 0xfe, 0xa5, 0x0e, 0x77, 0x27, 0x3c, 0x02, 0x97, 0x08, 0x6d,
      0x67, 0xe5, 0x81, 0x23, 0xe8 },
    { 0xba, 0x9b, 0xcf, 0xb3, 0x73, 0x1a, 0x6c, 0xb1, 0x7d, 0xba, 0x21, 0x9a, 0x8d, 0x37, 0xea,
      0x4d, 0x52, 0x33, 0x22, 0x56 },
    { 0xe8, 0xed, 0x31, 0x60, 0x5c, 0xc7, 0xd6, 0x66, 0x06, 0x91, 0x99, 0x8f, 0x02, 0x4e, 0xed,
      0x6b, 0xa8, 0xb4, 0xa3, 0x3f },
    { 0xbc, 0x03, 0x8e, 0x16, 0x3d, 0x21, 0xd9, 0xa1, 0x18, 0x1b, 0x60, 0xcf, 0x90, 0xb4, 0xd0,
      0x3e, 0xfa, 0xd9, 0xe0, 0xc5 },
    { 0x41, 0x80, 0x99, 0x82, 0x4a, 0xf6, 0xbf, 0x1c, 0xd7, 0xf8, 0xbb, 0x44, 0xf6, 0x1e, 0x3a,
      0x9c, 0xc3, 0x00, 0x7d, 0xae },
    { 0x4e, 0xc5, 0x38, 0x06, 0x5f, 0xdc, 0x8a, 0xcf, 0x49, 0x67, 0x43, 0x00, 0xcb, 0xde, 0xc5,
      0xb8, 0x0c, 0xc0, 0x5a, 0x0d },
    { 0xa4, 0xbe, 0xa4, 0x1b, 0xe8, 0x3d, 0x81, 0x6f, 0x79, 0x3f, 0x3f, 0xad, 0x97, 0xd2, 0x68,
      0xf7, 0x1a, 0xd9, 0x9b, 0xf9 },
    { 0x69, 0x2d, 0x56, 0x6b, 0x05, 0x43, 0x4d, 0x8c, 0x66, 0xa5, 0x5b, 0xdf, 0xf4, 0x86, 0x69,
      0x8e, 0x0f, 0xc9, 0x60, 0x95 },
    { 0x19, 0x41, 0xa2, 0x9a, 0xd2, 0xe7, 0xb7, 0xbb, 0xa8, 0xa2, 0x9e, 0x64, 0x40, 0xc9, 0x5e,
      0xf5, 0xcf, 0x76, 0xb0, 0x1d },
    { 0x07, 0x71, 0x11, 0xb9, 0x96, 0xe5, 0xc4, 0xf4, 0x7d, 0x20, 0xec, 0x29, 0xc2, 0x93, 0x85,
      0x04, 0xb5, 0x3a, 0x8e, 0x76 },
    { 0xf9, 0xa6, 0x2b, 0xef, 0x04, 0xcc, 0x8f, 0x49, 0x96, 0x33, 0xe4, 0x02, 0x3a, 0xcc, 0x76,
      0x75, 0xa9, 0x27, 0x71, 0xf0 },
    { 0xb0, 0x36, 0x86, 0x4c, 0xf0, 0x01, 0x6b, 0xe0, 0x58, 0x14, 0x49, 0xef, 0x29, 0xfb, 0x52,
      0xb2, 0xe5, 0x8d, 0x78, 0xa4 },
    { 0x15, 0xb3, 0x03, 0xb2, 0x88, 0xf4, 0x70, 0x7e, 0x5d, 0x0a, 0xf2, 0x83, 0x67, 0xc8, 0xce,
      0x51, 0xcd, 0xea, 0xb4, 0x90 },
};

static const char *courseAbbreviations[0x20] = {
    "LC",
    "MMM",
    "MG",
    "TF",
    "MC",
    "CM",
    "DKSC",
    "WGM",
    "DC",
    "KC",
    "MT",
    "GV",
    "DDR",
    "MH",
    "BC",
    "RR",
    "rPB",
    "rYF",
    "rGV2",
    "rMR",
    "rSL",
    "rSGB",
    "rDS",
    "rWS",
    "rDH",
    "rBC3",
    "rDKJP",
    "rMC",
    "rMC3",
    "rPG",
    "rDKM",
    "rBC",
};

static char nibbleToChar(u8 val) {
    if (val < 0xa) {
        return '0' + val;
    }
    return 'a' + val - 0xa;
}

static void getCourseName(const u8 *courseSha1, char *courseName) {
    for (u32 i = 0; i < 0x20; i++) {
        if (!memcmp(courseSha1, courseSha1s[i], 0x14)) {
            memcpy(courseName, courseAbbreviations[i], strlen(courseAbbreviations[i]) + 1);
            return;
        }
    }

    for (u32 i = 0; i < 2 * 0x14; i += 2) {
        courseName[i] = nibbleToChar(courseSha1[i / 2] >> 4);
        courseName[i + 1] = nibbleToChar(courseSha1[i / 2] & 0xf);
    }
    courseName[2 * 0x14] = '\0';
}

static void SaveManager_saveGhost(SaveManager *this, GhostFile *file) {
    OSJoinThread(&this->ghostInitThread, NULL);
    OSDetachThread(&this->ghostInitThread);

    this->saveGhostResult = false;

    if (file->raceTime.minutes > 99) {
        goto fail;
    }

    u32 size = GhostFile_spWrite(file, this->rawGhostFile);
    if (size == 0) {
        goto fail;
    }

    wchar_t path[255 + 1];
    u32 offset = swprintf(path, 255 + 1, L"/mkw-sp/ghosts/");

    char courseName[0x14 * 2 + 1];
    getCourseName(this->courseSha1s[file->courseId], courseName);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%s", courseName);

    if (!Storage_createDir(path, true)) {
        goto fail;
    }

    offset += swprintf(path + offset, 255 + 1 - offset, L"/%02um", file->raceTime.minutes);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%02us", file->raceTime.seconds);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%03u ", file->raceTime.milliseconds);
    const wchar_t *miiName = file->rawMii.name;
    if (miiName[0] == L'\0') {
        miiName = L"Player";
    }
    offset += swprintf(path + offset, 255 + 1 - offset, L"%ls", miiName);
    swprintf(path + offset, 255 + 1 - offset, L".rkg");

    if (!Storage_writeFile(path, false, this->rawGhostFile, size)) {
        u32 i;
        for (i = 0; i < 100; i++) {
            swprintf(path + offset, 255 + 1 - offset, L" %u.rkg", i);
            if (Storage_writeFile(path, false, this->rawGhostFile, size)) {
                break;
            }
        }
        if (i >= 100) {
            goto fail;
        }
    }

    this->saveGhostResult = true;

    NodeInfo info;
    Storage_stat(path, &info);
    if (info.type == NODE_TYPE_FILE) {
        SaveManager_initGhost(this, info.id);
    }

fail:
    this->isBusy = false;
}

static void saveGhostTask(void *arg) {
    GhostFile *file = arg;
    SaveManager_saveGhost(s_saveManager, file);
}

static void my_SaveManager_saveGhostAsync(SaveManager *this, s32 UNUSED(licenseId),
        u32 UNUSED(category), u32 UNUSED(index), GhostFile *file, bool UNUSED(saveLicense)) {
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

    SP_LOG("Computing course SHA1: %s\n", courseFilenames[courseId]);

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
            SP_LOG("DVDRead failed");
            goto cleanup;
        }
        fileOffset += srcSize;

        const u8 *src = srcBuffer;
        do {
            u8 *dst = dstBuffer;
            u32 dstSize = sizeof(dstBuffer);
            yazResult = YazDecoder_feed(&yazDecoder, &src, &dst, &srcSize, &dstSize);
            if (yazResult == YAZ_DECODER_RESULT_ERROR) {
                SP_LOG("YAZ_DECODER_RESULT_ERROR");
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

const u8 *SaveManager_getCourseSha1(const SaveManager *this, u32 courseId) {
    return this->courseSha1s[courseId];
}

bool vsSpeedModIsEnabled;
