#pragma once

#include <Common.hh>

namespace EGG {

class Xfb {
public:
    u16 width() const;
    u16 height() const;
    void *buffer();

    static u32 CalcXfbSize(u16 width, u16 height);

private:
    u16 m_width;
    u16 m_height;
    void *m_buffer;
    u8 _08[0x10 - 0x08];
};
static_assert(sizeof(Xfb) == 0x10);

} // namespace EGG
