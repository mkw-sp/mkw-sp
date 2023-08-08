#pragma once

#include "game/system/GhostFile.hh"
#include "game/system/Mii.hh"
#include "game/system/NandHelper.hh"

#include <egg/core/eggTaskThread.hh>
#include <sp/settings/ClientSettings.hh>
#include <sp/storage/Storage.hh>

namespace System {

class SaveManager {
private:
    struct RawLicense {
        REPLACE void reset();

        u8 _0000[0x0030 - 0x0000];
        u32 unlockFlags[4];
        u8 _0040[0x0080 - 0x0040];
        u16 rules[4];
        u8 _0088[0x00b0 - 0x0088];
        u16 vr;
        u16 br;
        u8 _00b4[0x00ea - 0x00b4];
        u16 driftMode;
        u8 _00ec[0x8cc0 - 0x00ec];
    };
    static_assert(sizeof(RawLicense) == 0x8cc0);

    struct RawSave {
        u8 _00000[0x2330c - 0x00000];
        u32 globalSettings : 30;
        bool m_flagDisplay : 1;
        bool m_rumble : 1;
        u8 _23310[0x28000 - 0x23310];
    };
    static_assert(sizeof(RawSave) == 0x28000);

    class License {
        wchar_t m_miiName[11];
        MiiId m_miiId;
        u8 _001e[0x93f0 - 0x001e];
    };
    static_assert(sizeof(License) == 0x93f0);

public:
    SaveManager();
    REPLACE void initAsync();
    REPLACE void resetAsync();
    bool isBusy() const;
    bool saveGhostResult() const;

    REPLACE void saveLicensesAsync();
    void eraseLicense(u32 licenseId);
    void createLicense(u32 licenseId, const MiiId *miiId, const wchar_t *miiName);
    REPLACE void selectLicense(u32 licenseId);

    u32 spLicenseCount() const;
    void eraseSPLicense();
    void createSPLicense(const MiiId *miiId);
    std::optional<u32> spCurrentLicense() const;
    void selectSPLicense(u32 licenseId);
    void unselectSPLicense();

    u32 getSetting(u32 setting) const;
    void setSetting(u32 setting, u32 value);

    template <SP::ClientSettings::Setting S>
    SP::ClientSettings::Helper<S>::type getSetting() const {
        if (!m_spCurrentLicense) {
            u32 defaultValue = SP::ClientSettings::entries[static_cast<u32>(S)].defaultValue;
            return static_cast<SP::ClientSettings::Helper<S>::type>(defaultValue);
        }

        auto vanillaValue = SP::ClientSettings::entries[static_cast<u32>(S)].vanillaValue;
        if (vanillaValue) {
            auto setting = getSetting<SP::ClientSettings::Setting::VanillaMode>();
            if (setting == SP::ClientSettings::VanillaMode::Enable) {
                return static_cast<SP::ClientSettings::Helper<S>::type>(*vanillaValue);
            }
        }

        return m_spLicenses[*m_spCurrentLicense].get<SP::ClientSettings::Setting, S>();
    }

    template <SP::ClientSettings::Setting S>
    void setSetting(SP::ClientSettings::Helper<S>::type value) {
        if (!m_spCurrentLicense) {
            return;
        }

        m_spLicenses[*m_spCurrentLicense].set<SP::ClientSettings::Setting, S>(value);

        refreshGCPadRumble();
        refreshRegionFlagDisplay();
    }

    void setSetting(const char *key, const char *value);

    void setMiiId(MiiId id);

    MiiId getMiiId(u32 licenseId) const;

    u32 ghostCount() const;
    RawGhostHeader *rawGhostHeader(u32 i);
    GhostFooter *ghostFooter(u32 i);
    REPLACE void loadGhostHeadersAsync(s32 licenseId, GhostGroup *group);
    REPLACE void loadGhostAsync(s32 licenseId, u32 category, u32 index, u32 courseId);
    REPLACE void saveGhostAsync(s32 licenseId, u32 category, u32 index, GhostFile *file,
            bool saveLicense);

    Sha1 courseSHA1(Registry::Course courseId) const;
    Sha1 vanillaSHA1(Registry::Course courseId) const;
    bool isCourseReplaced(Registry::Course courseId) const;

    void getLocation(u32 *location) const;
    void getLatitude(u16 *latitude) const;
    void getLongitude(u16 *longitude) const;

    static REPLACE SaveManager *CreateInstance();
    static SaveManager *Instance();

private:
    void init();
    void initSPSave();
    void initCourseSHA1s();
    void initGhostsAsync();
    void initGhosts();
    void initGhosts(const wchar_t *path);
    void initGhosts(SP::Storage::NodeId id);
    void initGhost(SP::Storage::NodeId id);

    void saveSPSave();
    void refreshGCPadRumble();
    void refreshRegionFlagDisplay();

    void loadGhostHeaders();
    void loadGhosts();
    bool loadGhost(u32 i);
    void saveGhost(GhostFile *file);

    static void InitTask(void *arg);
    static void *InitGhostsTask(void *arg);

    static void SaveSPSaveTask(void *arg);

    static void LoadGhostHeadersTask(void *arg);
    static void LoadGhostsTask(void *arg);
    static void SaveGhostTask(void *arg);

    static void GetCourseName(Sha1 courseSHA1, char (&courseName)[0x14 * 2 + 1]);

    u8 _00000[0x00014 - 0x00000];
    RawSave *m_rawSave;
    u8 *m_rawGhostFile;
    EGG::TaskThread *m_taskThread;
    u32 m_ghostCount;                  // Modified
    RawGhostHeader *m_rawGhostHeaders; // Modified
    GhostGroup *m_ghostGroup;
    GhostFooter *m_ghostFooters;     // Modified
    SP::Storage::NodeId *m_ghostIds; // Modified
    u8 _00034[0x00035 - 0x00034];
    bool m_saveGhostResult;
    s16 m_currentLicenseId;
    License m_licenses[4];
    u8 _24ff8[0x24ffc - 0x24ff8];
    RawSave *m_otherRawSave;
    bool m_isBusy;
    bool m_isValid;
    bool m_canSave;
    bool m_spCanSave; // Added (was padding)
    NandResult m_result;
    u32 m_spLicenseCount;                               // Added
    SP::ClientSettings::Settings m_spLicenses[6];       // Added
    std::optional<u8> m_spCurrentLicense;               // Added
    u8 m_ghostInitStack[0x8000 /* 32 KiB */];           // Added
    OSThread m_ghostInitThread;                         // Added
    std::array<std::array<u8, 0x14>, 32> m_courseSHA1s; // Added

    static SaveManager *s_instance;
    static const std::array<Sha1, 42> s_courseSHA1s;
    static const char *s_courseAbbreviations[32];

public:
    bool m_usedItemWheel; // Added
};

} // namespace System
