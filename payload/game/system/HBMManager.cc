#include "HBMManager.hh"

#include "game/host_system/SystemManager.hh"

#include <sp/storage/DecompLoader.hh>

namespace System {

u8 *HBMManager::getFile(const char *path, EGG::Heap *heap, bool isCompressed, u32 *fileSize) {
    if (isCompressed) {
        u8 *file;
        size_t tmpSize;
        bool result = SP::Storage::DecompLoader::LoadRO(path, &file, &tmpSize, heap);
        assert(result);
        if (fileSize) {
            *fileSize = tmpSize;
        }
        return file;
    } else {
        u8 *file = SystemManager::RipFromDisc(path, heap, false, fileSize);
        assert(file);
        return file;
    }
}

bool HBMManager::isActive() const {
    return m_isActive;
}

HBMManager *HBMManager::Instance() {
    return s_instance;
}

} // namespace System
