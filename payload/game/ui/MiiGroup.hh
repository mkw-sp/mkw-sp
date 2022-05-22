#pragma once

#include "game/system/Mii.hh"

#include <egg/core/eggHeap.hh>

namespace UI {

class MiiGroup {
public:
    MiiGroup();
    virtual ~MiiGroup();
    virtual void dt(s32 type);
    void init(size_t count, u32 presetFlags, EGG::Heap *heap);
    void insertFromId(size_t index, System::MiiId *id);
    System::Mii *get(size_t index);
    void copy(const MiiGroup *srcGroup, size_t srcIndex, size_t index);

private:
    u8 _00[0x98 - 0x04];
};
static_assert(sizeof(MiiGroup) == 0x98);

} // namespace UI
