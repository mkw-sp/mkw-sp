#pragma once

#include "game/system/GhostFile.hh"
#include "game/system/Mii.hh"

#include <egg/core/eggTaskThread.hh>
#include <sp/settings/ClientSettings.hh>

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
    void selectLicense(u32 licenseId);

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
            return GetDefault<typename SP::ClientSettings::Helper<S>::type>();
        }

        return m_spLicenses[*m_spCurrentLicense].get<SP::ClientSettings::Setting, S>();
    }

    template <SP::ClientSettings::Setting S>
    void setSetting(SP::ClientSettings::Helper<S>::type value) {
        if (!m_spCurrentLicense) {
            return;
        }

        m_spLicenses[*m_spCurrentLicense].set<SP::ClientSettings::Setting, S>(value);
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

    bool computeCourseSHA1Async(u32 courseId);
    const u8 *courseSHA1(u32 courseId) const;

    static REPLACE SaveManager *CreateInstance();
    static SaveManager *Instance();

private:
    void init();
    void initSPSave();
    void initGhostsAsync();
    void initGhosts();
    void initGhosts(wchar_t *path, u32 offset);
    void initGhost(NodeId id);

    void saveSPSave();

    void loadGhostHeaders();
    void loadGhosts();
    bool loadGhost(u32 i);
    void saveGhost(GhostFile *file);

    void computeCourseSHA1(u32 courseId);

    static void InitTask(void *arg);
    static void *InitGhostsTask(void *arg);

    static void SaveSPSaveTask(void *arg);

    static void LoadGhostHeadersTask(void *arg);
    static void LoadGhostsTask(void *arg);
    static void SaveGhostTask(void *arg);

    static void GetCourseName(const u8 *courseSHA1, char (&courseName)[0x14 * 2 + 1]);
    static void ComputeCourseSHA1Task(void *arg);

    template <typename T>
    static T GetDefault() {
        return T::Default;
    }

    u8 _00000[0x00014 - 0x00000];
    void *m_rawSave;
    u8 *m_rawGhostFile;
    EGG::TaskThread *m_taskThread;
    u32 m_ghostCount; // Modified
    RawGhostHeader *m_rawGhostHeaders; // Modified
    GhostGroup *m_ghostGroup;
    GhostFooter *m_ghostFooters; // Modified
    NodeId *m_ghostIds; // Modified
    u8 _00034[0x00035 - 0x00034];
    bool m_saveGhostResult;
    s16 m_currentLicenseId;
    License m_licenses[4];
    u8 _24ff8[0x24ffc - 0x24ff8];
    void *m_otherRawSave;
    bool m_isBusy;
    bool m_isValid;
    bool m_canSave;
    bool m_spCanSave; // Added (was padding)
    u32 m_result;
    u32 m_spLicenseCount; // Added
    SP::ClientSettings::Settings m_spLicenses[6]; // Added
    std::optional<u8> m_spCurrentLicense; // Added
    OSThread m_ghostInitThread; // Added
    u8 m_ghostInitStack[0x8000 /* 32 KiB */]; // Added
    bool m_courseSHA1IsValid[32]; // Added
    u8 m_courseSHA1s[32][0x14]; // Added

    static SaveManager *s_instance;
    static const u8 s_courseSHA1s[32][0x14];
    static const char *s_courseAbbreviations[32];
};

template <>
inline u32 SaveManager::GetDefault() {
    return 0;
}

} // namespace System
