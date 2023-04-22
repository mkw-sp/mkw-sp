#pragma once

#include <egg/core/eggHeap.hh>

namespace System {

class HBMManager {
public:
    bool isActive() const;
    REPLACE u8 *getFile(const char *path, EGG::Heap *heap, bool isCompressed, u32 *fileSize);

    static HBMManager *Instance();

private:
    u8 _000[0x04c - 0x000];
    bool m_isActive;
    u8 _04d[0x2b4 - 0x04d];

    static HBMManager *s_instance;
};
static_assert(sizeof(HBMManager) == 0x2b4);

} // namespace System
