#pragma once

#include "game/obj/Obj.hh"

namespace Geo {

class ObjPylon01 : public Obj {
public:
    REPLACE PixelMode pixelMode() const;

private:
    u8 _0b0[0x108 - 0x0b0];
    u32 m_playerId;
    u8 _10c[0x118 - 0x10c];
};
static_assert(sizeof(ObjPylon01) == 0x118);

} // namespace Geo
