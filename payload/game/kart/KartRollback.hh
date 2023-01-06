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

    std::optional<Frame> serverFrame(u32 playerId) const;
    void handleFutureFrame(const Frame &frame);
    void handlePastFrame(const Frame &frame);
    void applyFrame(const Frame &frame);

    SP::CircularBuffer<Frame, 60> m_frames;
    Vec3 m_posDelta{};
    Quat m_mainRotDelta{};
    f32 m_internalSpeedDelta = 0.0f;
};

} // namespace Kart
