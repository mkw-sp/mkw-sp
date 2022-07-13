#include "eggDVDRipper.hh"

#include <sp/storage/Storage.hh>

namespace EGG {

u8 *DVDRipper::LoadToMainRAM(const char *path, u8 *dst, Heap *heap, AllocDirection allocDirection,
        u32 offset, u32 *readSize, u32 *fileSize) {
    auto file = SP::Storage::OpenRO(path);
    if (!file) {
        return nullptr;
    }

    if (fileSize) {
        *fileSize = file->size();
    }

    u32 size = file->size() - offset;

    void *block = nullptr;
    if (!dst) {
        block = heap->alloc(size, allocDirection == AllocDirection::Top ? 0x20 : -0x20);
        if (!block) {
            return nullptr;
        }
        dst = reinterpret_cast<u8 *>(block);
    }

    if (!file->read(dst, size, offset)) {
        if (block) {
            heap->free(block);
        }
        return nullptr;
    }

    if (readSize) {
        *readSize = size;
    }

    return dst;
}

} // namespace EGG
