#pragma once

#include "KartObjectManager.hh"
#include "KartPart.hh"

namespace Kart {

class WheelPhysics : public KartObjectProxy {
    friend class KartSaveState;

private:
    WheelPhysics();
    virtual ~WheelPhysics();
    virtual void dt(s32 type);

    u32 m_wheelIdx;
    u32 m_bspWheelIdx;
    void *m_bspWheel;
    void *m_colisionGroup;
    Vec3 m_realPos;
    Vec3 m_lastPos;
    Vec3 m_lastPosDiff;
    f32 m_yDown;
    Vec3 _48;
    Vec3 m_speed;

    u8 _10[0x84 - 0x60];
};

class KartTire : public KartPart {
    friend class KartSaveState;

private:
    u8 _90[0x98 - 0x90];
    WheelPhysics *m_wheelPhysics;
    u8 _9c[0xd0 - 0x9c];
};

static_assert(sizeof(WheelPhysics) == 0x84);
static_assert(sizeof(KartTire) == 0xd0);

} // namespace Kart
