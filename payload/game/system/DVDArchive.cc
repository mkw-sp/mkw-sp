#include "DVDArchive.hh"

#include <sp/ThumbnailManager.hh>

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

    return REPLACED(getFile)(path, size);
}

} // namespace System
