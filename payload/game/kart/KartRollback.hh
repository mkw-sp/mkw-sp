#pragma once

#include "game/kart/KartObjectProxy.hh"

#include <sp/CircularBuffer.hh>

namespace Kart {

class KartRollback : public KartObjectProxy {
public:
    KartRollback();

    Vec3<f32> posDelta() const;
    Quat<f32> mainRotDelta() const;
    void calcEarly();
    void calcLate();

private:
    struct Frame {
        u32 id;
        Vec3<f32> pos;
        Quat<f32> mainRot;
    };

    SP::CircularBuffer<Frame, 60> m_frames;
    Vec3<f32> m_posDelta{};
    Quat<f32> m_mainRotDelta{};
};

} // namespace Kart
