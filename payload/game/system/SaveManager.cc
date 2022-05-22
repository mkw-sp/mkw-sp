#include "SaveManager.hh"

extern "C" {
#include "SaveManager.h"
}

#include <bit>

namespace System {

void SaveManager::createLicense(u32 licenseId, const MiiId *miiId, const wchar_t *miiName) {
    SaveManager_createLicense(s_saveManager, licenseId, reinterpret_cast<const ::MiiId *>(miiId),
            miiName);
}

void SaveManager::selectLicense(u32 licenseId) {
    SaveManager_selectLicense(s_saveManager, licenseId);
}

u32 SaveManager::spLicenseCount() const {
    return s_saveManager->spLicenseCount;
}

MiiId SaveManager::getSpLicenseMiiId(u32 spLicenseId) const {
    return std::bit_cast<MiiId>(SaveManager_getMiiId(s_saveManager, spLicenseId));
}

void SaveManager::eraseSpLicense() {
    SaveManager_eraseSpLicense(s_saveManager);
}

void SaveManager::createSpLicense(const MiiId *miiId) {
    SaveManager_createSpLicense(s_saveManager, reinterpret_cast<const ::MiiId *>(miiId));
}

std::optional<u32> SaveManager::spCurrentLicense() const {
    if (s_saveManager->spCurrentLicense < 0) {
        return {};
    }

    return s_saveManager->spCurrentLicense;
    //return s_saveManager->spCurrentLicense < 0 ? std::nullopt : s_saveManager->spCurrentLicense;
}

void SaveManager::selectSpLicense(u32 spLicenseId) {
    s_saveManager->spCurrentLicense = spLicenseId;
}

void SaveManager::unselectSpLicense() {
    s_saveManager->spCurrentLicense = -1;
}

SaveManager *SaveManager::Instance() {
    return nullptr;
}

} // namespace System
