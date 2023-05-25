#pragma once

#include "game/obj/Obj.hh"

#include <egg/util/eggEffect.hh>

namespace Battle {

class BalloonUnit : public Geo::ObjEntity {
public:
    BalloonUnit(u8 balloonId, u8 teamId);
    ~BalloonUnit() override;
    // ...
    void REPLACED(vf_1c)();
    REPLACE void vf_1c() override;
    // ...
    const char *REPLACED(vf_38)();
    REPLACE const char *vf_38();
    // ...
    void REPLACED(vf_48)();
    REPLACE void vf_48();
    // ...
    REPLACE void vf_58();
    // ...

    void REPLACED(onAdd)(u32 r4, u8 playerId, u8 r6, u8 r7);
    REPLACE void onAdd(u32 r4, u8 playerId, u8 r6, u8 r7);

private:
    u8 _0b0[0x0b3 - 0x0b0];
    u8 m_playerId; // Added (was padding)
    u8 _0b4[0x1d8 - 0x0b4];
    EGG::Effect **m_effect;
    u8 _1dc[0x1e4 - 0x1dc];
    nw4r::g3d::ScnMdl::CopiedMatAccess *m_copiedMatAccesses[2]; // Added
    nw4r::g3d::ResMatTevColor *m_resMatTevColors[2];            // Added
};
static_assert(sizeof(BalloonUnit) == 0x1e4 + sizeof(void *) * 2 * 2);

} // namespace Battle
