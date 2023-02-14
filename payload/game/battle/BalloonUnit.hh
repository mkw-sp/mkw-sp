#pragma once

#include "game/obj/Obj.hh"

namespace Battle {

class BalloonUnit : public Geo::ObjEntity {
public:
    BalloonUnit(u8 balloonId, u8 teamId);
    ~BalloonUnit() override;
    // ...
    const char *REPLACED(vf_38)();
    REPLACE const char *vf_38();
    // ...
    void REPLACED(vf_48)();
    REPLACE void vf_48();
    // ...

    void REPLACED(onAdd)(u32 r4, u8 playerId, u8 r6, u8 r7);
    REPLACE void onAdd(u32 r4, u8 playerId, u8 r6, u8 r7);

private:
    u8 _0b0[0x1e4 - 0x0b0];
    nw4r::g3d::ScnMdl::CopiedMatAccess *m_copiedMatAccesses[2]; // Added
    nw4r::g3d::ResMatTevColor *m_resMatTevColors[2];            // Added
};
static_assert(sizeof(BalloonUnit) == 0x1e4 + sizeof(void *) * 2 * 2);

} // namespace Battle
