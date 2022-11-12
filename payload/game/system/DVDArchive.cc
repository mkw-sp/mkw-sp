#include "DVDArchive.hh"

#include <sp/ThumbnailManager.hh>

#include <sp/settings/ClientSettings.hh>

#include "game/system/SaveManager.hh"

#include <cstring>

namespace System {

void *DVDArchive::getFile(const char *path, size_t *size) {
    if (SP::ThumbnailManager::IsActive()) {
        if (!strcmp(path, "race_camera.bcp") || !strcmp(path, "start_camera.bcp")) {
            return nullptr;
        }

        if (!strcmp(path, "kartCameraParam.bin")) {
            return REPLACED(getFile)("kartCameraParamThumbnails.bin", size);
        }
    }

    auto* saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();
    if (setting == SP::ClientSettings::VSMegaClouds::Enable) {
        if (!strcmp(path, "kumo.brres")) {
            return REPLACED(getFile)("MegaTC.brres", size);
        }
    }

    return REPLACED(getFile)(path, size);
}

} // namespace System
