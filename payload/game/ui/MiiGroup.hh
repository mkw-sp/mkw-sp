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
    REPLACE void insertFromId(size_t index, const System::MiiId *id);
    void REPLACED(insertFromId)(size_t index, const System::MiiId *id);
    void insertFromRaw(u32 index, const System::RawMii *raw);
    System::Mii *get(size_t index);
    void copy(const MiiGroup *srcGroup, size_t srcIndex, size_t index);
    void swap(u32 i0, u32 i1);

private:
    struct Texture {
        u8 _00[0x24 - 0x00];
    };
    static_assert(sizeof(Texture) == 0x24);

    System::Mii **m_miis;
    Texture *m_textures[7];
    u8 _24[0x28 - 0x24];
    u32 m_miiCount;
    u8 _2C[0x98 - 0x2C];
};
static_assert(sizeof(MiiGroup) == 0x98);

} // namespace UI
