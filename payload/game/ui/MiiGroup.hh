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
    void insertFromRaw(u32 index, const System::RawMii *raw);
    System::Mii *get(size_t index);
    void copy(const MiiGroup *srcGroup, size_t srcIndex, size_t index);
    void swap(u32 i0, u32 i1);
    void clear();

private:
    struct Texture {
        u8 _00[0x24 - 0x00];
    };
    static_assert(sizeof(Texture) == 0x24);

    System::Mii **m_miis;
    Texture *m_textures[7];
    u8 _08[0x98 - 0x24];
};
static_assert(sizeof(MiiGroup) == 0x98);

} // namespace UI
