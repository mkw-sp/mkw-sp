#pragma once

#include "game/system/Mii.hh"

#include <optional>

namespace System {

class SaveManager {
public:
    void createLicense(u32 licenseId, const MiiId *miiId, const wchar_t *miiName);
    void selectLicense(u32 licenseId);
    u32 spLicenseCount() const;
    MiiId getSpLicenseMiiId(u32 spLicenseId) const;
    void eraseSpLicense();
    void createSpLicense(const MiiId *miiId);
    std::optional<u32> spCurrentLicense() const;
    void selectSpLicense(u32 spLicenseId);
    void unselectSpLicense();

    static SaveManager *Instance();
};

} // namespace System
