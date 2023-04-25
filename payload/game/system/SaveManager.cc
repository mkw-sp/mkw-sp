#include "SaveManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RootScene.hh"
extern "C" {
#include "game/system/SaveManager.h"
}
#include "game/ui/SectionManager.hh"

#include <common/Bytes.hh>
#include <sp/storage/DecompLoader.hh>

#include <bit>
#include <cstring>

namespace System {

void SaveManager::RawLicense::reset() {
    *this = {};
    unlockFlags[0] = 0xffffffff;
    unlockFlags[1] = 0x31ffffc;
    rules[0] = 0x5008;
    rules[1] = 0x1000;
    rules[2] = 0x5008;
    rules[3] = 0x1000;
    vr = 5000;
    br = 5000;
    auto value = SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::DriftMode>();
    driftMode = (static_cast<u32>(value) + 1) << 14;
}

SaveManager *SaveManager::CreateInstance() {
    auto *heap = RootScene::Instance()->m_heapCollection.mem1;

    s_instance = new (heap, 0x4) SaveManager;
    assert(s_instance);

    s_instance->m_ghostCount = 0;
    s_instance->m_rawGhostHeaders = new (heap, 0x4) RawGhostHeader[MAX_GHOST_COUNT];
    s_instance->m_ghostFooters = new (heap, 0x4) GhostFooter[MAX_GHOST_COUNT];
    s_instance->m_ghostIds = new (heap, 0x4) SP::Storage::NodeId[MAX_GHOST_COUNT];

    s_instance->m_spCanSave = true;
    s_instance->m_spLicenseCount = 0;
    for (u32 i = 0; i < std::size(s_instance->m_spLicenses); i++) {
        s_instance->m_spLicenses[i] = SP::ClientSettings::Settings();
        s_instance->m_spLicenses[i].reset();
    }
    s_instance->m_spCurrentLicense = {};

    return s_instance;
}

void SaveManager::initAsync() {
    m_isBusy = true;
    m_taskThread->request(InitTask, nullptr, nullptr);
}

void SaveManager::InitTask(void * /* arg */) {
    assert(s_instance);
    s_instance->init();
}

void SaveManager::init() {
    m_isValid = true;
    m_canSave = false;

    m_otherRawSave = m_rawSave;

    initSPSave();
    initGhostsAsync();

    m_isBusy = false;
}

void SaveManager::initSPSave() {
    // TODO: Hopefully this is enough. Can always stream the file if not.
    char iniBuffer[2048];

    for (m_spLicenseCount = 0; m_spLicenseCount < std::size(m_spLicenses);) {
        wchar_t path[64];
        swprintf(path, std::size(path), L"/mkw-sp/settings%u.ini", m_spLicenseCount);

        auto size = SP::Storage::ReadFile(path, iniBuffer, sizeof(iniBuffer));
        if (!size) {
            break;
        }

        m_spLicenses[m_spLicenseCount++].readIni(iniBuffer, *size);
    }
}

void SaveManager::initGhostsAsync() {
    u8 *stackTop = m_ghostInitStack + sizeof(m_ghostInitStack);
    u32 stackSize = sizeof(m_ghostInitStack);
    OSCreateThread(&m_ghostInitThread, InitGhostsTask, nullptr, stackTop, stackSize, 31, 0);
    OSResumeThread(&m_ghostInitThread);
}

void *SaveManager::InitGhostsTask(void * /* arg */) {
    assert(s_instance);
    s_instance->initGhosts();
    return nullptr;
}

void SaveManager::initGhosts() {
    SP_LOG("Initializing ghosts...");

    initGhosts(L"/mkw-sp/ghosts");
    initGhosts(L"/ctgpr/ghosts");

    SP::Storage::CreateDir(L"/mkw-sp/ghosts", true);

    SP_LOG("Ghosts: %u / %u", m_ghostCount, MAX_GHOST_COUNT);
}

void SaveManager::initGhosts(const wchar_t *path) {
    auto info = SP::Storage::Stat(path);
    if (!info) {
        return;
    }
    if (info->type != SP::Storage::NodeType::Dir) {
        return;
    }
    initGhosts(info->id);
}

void SaveManager::initGhosts(SP::Storage::NodeId id) {
    if (m_ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    auto dir = SP::Storage::FastOpenDir(id);
    if (!dir) {
        return;
    }

    while (auto info = dir->read()) {
        if (info->type == SP::Storage::NodeType::Dir) {
            initGhosts(info->id);
        } else {
            initGhost(info->id);
        }
    }
}

void SaveManager::initGhost(SP::Storage::NodeId id) {
    if (m_ghostCount >= MAX_GHOST_COUNT) {
        return;
    }

    auto readSize = SP::Storage::FastReadFile(id, m_rawGhostFile, 0x2800);
    if (!readSize) {
        return;
    }

    if (!RawGhostFile::IsValid(m_rawGhostFile, *readSize)) {
        return;
    }

    auto *header = reinterpret_cast<const RawGhostHeader *>(m_rawGhostFile);
    memcpy(&m_rawGhostHeaders[m_ghostCount], header, sizeof(RawGhostHeader));
    m_ghostFooters[m_ghostCount] = GhostFooter(m_rawGhostFile, *readSize);
    m_ghostIds[m_ghostCount] = id;
    m_ghostCount++;
}

void SaveManager::resetAsync() {
    m_isValid = true;
    m_canSave = false;

    m_isBusy = false;
    m_result = NandResult::Ok;
}

bool SaveManager::isBusy() const {
    return m_isBusy;
}

bool SaveManager::saveGhostResult() const {
    return m_saveGhostResult;
}

void SaveManager::saveLicensesAsync() {
    if (m_spCurrentLicense) {
        auto *context = UI::SectionManager::Instance()->globalContext();

        Registry::Character character = context->m_localCharacterIds[0];
        Registry::Vehicle vehicle = context->m_localVehicleIds[0];
        setSetting<SP::ClientSettings::Setting::Character>(character);
        setSetting<SP::ClientSettings::Setting::Vehicle>(vehicle);

        auto driftMode = static_cast<SP::ClientSettings::DriftMode>(context->m_driftModes[0] - 1);
        setSetting<SP::ClientSettings::Setting::DriftMode>(driftMode);
    }

    if (!m_spCanSave) {
        m_isBusy = false;
        m_result = NandResult::Ok;
    }

    m_isBusy = true;
    m_taskThread->request(SaveSPSaveTask, nullptr, nullptr);
}

void SaveManager::SaveSPSaveTask(void * /* arg */) {
    assert(s_instance);
    s_instance->saveSPSave();
}

void SaveManager::saveSPSave() {
    char iniBuffer[2048];

    for (size_t i = 0; i < m_spLicenseCount; ++i) {
        m_spLicenses[i].writeIni(iniBuffer, sizeof(iniBuffer));

        wchar_t path[64];
        swprintf(path, std::size(path), L"/mkw-sp/settings%u.ini", (unsigned)i);

        if (!SP::Storage::WriteFile(path, iniBuffer, strlen(iniBuffer), true)) {
            SP_LOG("Failed to save %ls", path);
            m_spCanSave = false;
            m_isBusy = false;
            m_result = NandResult::NoSpace;
            return;
        }
    }

    for (size_t i = m_spLicenseCount; i < 6; ++i) {
        wchar_t path[64];
        swprintf(path, std::size(path), L"/mkw-sp/settings%u.ini", (unsigned)i);

        if (!SP::Storage::Remove(path, true)) {
            SP_LOG("Failed to delete %ls", path);
            m_spCanSave = false;
            m_isBusy = false;
            m_result = NandResult::NoSpace;
            return;
        }
    }

    m_isBusy = false;
    m_result = NandResult::Ok;
}

void SaveManager::selectLicense(u32 licenseId) {
    m_currentLicenseId = licenseId;
}

void SaveManager::refreshGCPadRumble() {
    auto setting = getSetting<SP::ClientSettings::Setting::GCPadRumble>();
    m_rawSave->m_rumble = setting == SP::ClientSettings::GCPadRumble::Enable;
}

void SaveManager::refreshRegionFlagDisplay() {
    auto setting = getSetting<SP::ClientSettings::Setting::RegionFlagDisplay>();
    m_rawSave->m_flagDisplay = setting == SP::ClientSettings::RegionFlagDisplay::Enable;
}

u32 SaveManager::spLicenseCount() const {
    return m_spLicenseCount;
}

void SaveManager::eraseSPLicense() {
    if (!m_spCurrentLicense) {
        return;
    }
    for (u32 i = *m_spCurrentLicense; i < m_spLicenseCount - 1; i++) {
        m_spLicenses[i] = m_spLicenses[i + 1];
    }
    m_spLicenseCount--;
    m_spCurrentLicense.reset();
}

void SaveManager::createSPLicense(const MiiId *miiId) {
    SP::ClientSettings::Settings &license = m_spLicenses[m_spLicenseCount++];
    license.reset();
    m_spCurrentLicense = m_spLicenseCount - 1;
    setMiiId(*miiId);
}

std::optional<u32> SaveManager::spCurrentLicense() const {
    return m_spCurrentLicense;
}

void SaveManager::selectSPLicense(u32 licenseId) {
    m_spCurrentLicense = licenseId;

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

void SaveManager::unselectSPLicense() {
    m_spCurrentLicense.reset();

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

u32 SaveManager::getSetting(u32 setting) const {
    if (!m_spCurrentLicense) {
        return SP::ClientSettings::entries[setting].defaultValue;
    }

    auto vanillaValue = SP::ClientSettings::entries[setting].vanillaValue;
    if (vanillaValue) {
        auto vanillaModeSetting = getSetting<SP::ClientSettings::Setting::VanillaMode>();
        if (vanillaModeSetting == SP::ClientSettings::VanillaMode::Enable) {
            return *vanillaValue;
        }
    }

    return m_spLicenses[*m_spCurrentLicense].get(setting);
}

void SaveManager::setSetting(u32 setting, u32 value) {
    if (!m_spCurrentLicense) {
        return;
    }

    m_spLicenses[*m_spCurrentLicense].set(setting, value);

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

void SaveManager::setSetting(const char *key, const char *value) {
    if (!m_spCurrentLicense) {
        return;
    }

    m_spLicenses[*m_spCurrentLicense].set(key, value);

    refreshGCPadRumble();
    refreshRegionFlagDisplay();
}

void SaveManager::setMiiId(MiiId miiId) {
    u32 avatar = Bytes::Read<u32>(miiId.avatar, 0);
    u32 client = Bytes::Read<u32>(miiId.client, 0);
    SP::ClientSettings::Settings &license = m_spLicenses[*m_spCurrentLicense];
    license.set<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiAvatar>(avatar);
    license.set<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiClient>(client);
}

System::MiiId SaveManager::getMiiId(u32 licenseId) const {
    const SP::ClientSettings::Settings &license = m_spLicenses[licenseId];
    u32 avatar = license.get<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiAvatar>();
    u32 client = license.get<SP::ClientSettings::Setting, SP::ClientSettings::Setting::MiiClient>();
    MiiId miiId{};
    Bytes::Write<u32>(miiId.avatar, 0, avatar);
    Bytes::Write<u32>(miiId.client, 0, client);
    return miiId;
}

u32 SaveManager::ghostCount() const {
    return m_ghostCount;
}

RawGhostHeader *SaveManager::rawGhostHeader(u32 i) {
    return &m_rawGhostHeaders[i];
}

GhostFooter *SaveManager::ghostFooter(u32 i) {
    return &m_ghostFooters[i];
}

void SaveManager::loadGhostHeadersAsync(s32 /* licenseId */, GhostGroup * /* group */) {
    m_isBusy = true;
    m_taskThread->request(LoadGhostHeadersTask, nullptr, nullptr);
}

void SaveManager::LoadGhostHeadersTask(void * /* arg */) {
    assert(s_instance);
    s_instance->loadGhostHeaders();
}

void SaveManager::loadGhostHeaders() {
    if (OSJoinThread(&m_ghostInitThread, nullptr)) {
        OSDetachThread(&m_ghostInitThread);
    }

    m_isBusy = false;
    m_result = NandResult::Ok;
}

void SaveManager::loadGhostAsync(s32 /* licenseId */, u32 /* category */, u32 /* index */,
        u32 /* courseId */) {
    m_isBusy = true;
    m_taskThread->request(LoadGhostsTask, nullptr, nullptr);
}

void SaveManager::LoadGhostsTask(void * /* arg */) {
    assert(s_instance);
    s_instance->loadGhosts();
}

void SaveManager::loadGhosts() {
    if (OSJoinThread(&m_ghostInitThread, nullptr)) {
        OSDetachThread(&m_ghostInitThread);
    }

    auto *raceConfig = RaceConfig::Instance();
    auto &raceScenario = raceConfig->raceScenario();
    auto &menuScenario = raceConfig->menuScenario();
    if (menuScenario.ghostBuffer == raceScenario.ghostBuffer) {
        if (menuScenario.ghostBuffer == raceConfig->ghostBuffers() + 0) {
            menuScenario.ghostBuffer = raceConfig->ghostBuffers() + 1;
        } else {
            menuScenario.ghostBuffer = raceConfig->ghostBuffers() + 0;
        }
    }

    auto *context = UI::SectionManager::Instance()->globalContext();
    for (u32 i = 0; i < context->m_timeAttackGhostCount; i++) {
        if (!loadGhost(i)) {
            memset((*menuScenario.ghostBuffer)[i], 0, 0x2800);
        }
    }

    m_isBusy = false;
}

bool SaveManager::loadGhost(u32 i) {
    auto *context = UI::SectionManager::Instance()->globalContext();
    auto &menuScenario = RaceConfig::Instance()->menuScenario();

    auto id = m_ghostIds[context->m_timeAttackGhostIndices[i]];
    auto readSize = SP::Storage::FastReadFile(id, m_rawGhostFile, 0x2800);
    if (!readSize) {
        return false;
    }

    if (reinterpret_cast<RawGhostHeader *>(m_rawGhostFile)->isCompressed) {
        if (!RawGhostFile::IsValid(m_rawGhostFile, *readSize)) {
            return false;
        }

        if (!RawGhostFile::Decompress(m_rawGhostFile, (*menuScenario.ghostBuffer)[i])) {
            return false;
        }
    } else {
        memcpy((*menuScenario.ghostBuffer)[i], m_rawGhostFile, 0x2800);
    }

    return RawGhostFile::IsValid((*menuScenario.ghostBuffer)[i], 0x2800);
}

void SaveManager::saveGhostAsync(s32 /* licenseId */, u32 /* category */, u32 /* index */,
        GhostFile *file, bool /* saveLicense */) {
    m_isBusy = true;
    m_taskThread->request(SaveGhostTask, file, nullptr);
}

void SaveManager::SaveGhostTask(void *arg) {
    assert(s_instance);
    auto file = reinterpret_cast<GhostFile *>(arg);
    s_instance->saveGhost(file);
    s_instance->m_isBusy = false;
}

void SaveManager::saveGhost(GhostFile *file) {
    if (OSJoinThread(&m_ghostInitThread, nullptr)) {
        OSDetachThread(&m_ghostInitThread);
    }

    auto &packInfo = RaceConfig::Instance()->m_packInfo;
    auto sha1 = packInfo.getSelectedSha1();
    auto sha1Hex = sha1ToHex(sha1);

    m_saveGhostResult = false;
    SPFooter::OnRaceEnd(sha1);

    Time raceTime = file->raceTime();
    if (raceTime.minutes > 99) {
        return;
    }

    auto size = file->write(m_rawGhostFile);
    if (!size) {
        return;
    }

    wchar_t path[255 + 1];
    u32 offset = swprintf(path, 255 + 1, L"/mkw-sp/ghosts/");

    offset += swprintf(path + offset, 255 + 1 - offset, L"%s", sha1Hex);

    if (!SP::Storage::CreateDir(path, true)) {
        return;
    }

    offset += swprintf(path + offset, 255 + 1 - offset, L"/%02um", raceTime.minutes);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%02us", raceTime.seconds);
    offset += swprintf(path + offset, 255 + 1 - offset, L"%03u ", raceTime.milliseconds);
    RawMii mii = file->rawMii();
    const wchar_t *miiName = mii.name;
    if (miiName[0] == L'\0') {
        miiName = L"Player";
    }
    offset += swprintf(path + offset, 255 + 1 - offset, L"%ls", miiName);
    swprintf(path + offset, 255 + 1 - offset, L".rkg");

    if (!SP::Storage::WriteFile(path, m_rawGhostFile, *size, false)) {
        u32 i;
        for (i = 0; i < 100; i++) {
            swprintf(path + offset, 255 + 1 - offset, L" %u.rkg", i);
            if (SP::Storage::WriteFile(path, m_rawGhostFile, *size, false)) {
                break;
            }
        }
        if (i >= 100) {
            return;
        }
    }

    m_saveGhostResult = true;

    auto info = SP::Storage::Stat(path);
    if (info && info->type == SP::Storage::NodeType::File) {
        initGhost(info->id);
    }
}

SaveManager *SaveManager::Instance() {
    return s_instance;
}

} // namespace System

extern "C" {
void SaveManager_EraseLicense(u32 licenseId) {
    return System::SaveManager::Instance()->eraseLicense(licenseId);
}

void SaveManager_EraseSPLicense(void) {
    System::SaveManager::Instance()->eraseSPLicense();
}

void SaveManager_CreateSPLicense(const MiiId *miiId) {
    System::SaveManager::Instance()->createSPLicense(
            reinterpret_cast<const System::MiiId *>(miiId));
}

s32 SaveManager_SPCurrentLicense(void) {
    return System::SaveManager::Instance()->spCurrentLicense().value_or(-1);
}

u32 SaveManager_GetVanillaMode(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetFOV169(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::FOV169>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetMapIcons(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::MapIcons>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTAClass(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TAClass>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTAGhostTagVisibility(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TAGhostTagVisibility>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTASolidGhosts(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TASolidGhosts>();
    return static_cast<u32>(value);
}

u32 SaveManager_GetTAGhostSound(void) {
    auto *saveManager = System::SaveManager::Instance();
    auto value = saveManager->getSetting<SP::ClientSettings::Setting::TAGhostSound>();
    return static_cast<u32>(value);
}

void SaveManager_SetMiiId(const MiiId *miiId) {
    auto *saveManager = System::SaveManager::Instance();
    saveManager->setMiiId(*std::bit_cast<System::MiiId *>(miiId));
}

bool vsSpeedModIsEnabled;
}
