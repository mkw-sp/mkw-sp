#pragma once

#include "game/kart/KartObjectProxy.hh"

#include <sp/CircularBuffer.hh>

namespace Kart {

class KartRollback : public KartObjectProxy {
public:
    KartRollback();

    Vec3 posDelta() const;
    Quat mainRotDelta() const;
    f32 internalSpeedDelta() const;
    void calcEarly();
    void calcLate();

private:
    struct Frame {
        u32 time;
        s16 timeBeforeRespawn;
        s16 timeInRespawn;
        Vec3 pos;
        Quat mainRot;
        f32 internalSpeed;
    };

    SP::CircularBuffer<Frame, 60> m_frames;
    Vec3 m_posDelta{};
    Quat m_mainRotDelta{};
    f32 m_internalSpeedDelta = 0.0f;
};

} // namespace Kart
