#pragma once

#include <revolution.hh>

namespace nw4r::lyt {

class TexMap {
public:
    TexMap(const GXTexObj &texObj);
    void set(const GXTexObj &texObj);
    void replaceImage(void *tpl, u32 index);

private:
    u8 _00[0x04 - 0x00];
    u32 _04;
    u8 _08[0x16 - 0x08];
    u16 _16;
    u32 _18;
};
static_assert(sizeof(TexMap) == 0x1c);

} // namespace nw4r::lyt
