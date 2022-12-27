#pragma once

#include "game/kart/KartObjectProxy.hh"

#include <sp/CircularBuffer.hh>

namespace Kart {

class KartRollback : public KartObjectProxy {
public:
    KartRollback();

    Vec3 posDelta() const;
    Quat mainRotDelta() const;
    void calcEarly();
    void calcLate();

private:
    struct Frame {
        u32 id;
        Vec3 pos;
        Quat mainRot;
    };

    SP::CircularBuffer<Frame, 60> m_frames;
    Vec3 m_posDelta{};
    Quat m_mainRotDelta{};
};

} // namespace Kart
