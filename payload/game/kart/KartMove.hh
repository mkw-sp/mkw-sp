#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
    friend class KartRollback;

public:
    f32 hardSpeedLimit() const;
    const Vec3<f32> *internalVelDir() const;
    void activateMega();
    void REPLACED(calcBlink)();
    REPLACE void calcBlink();
    bool REPLACED(activateTcLightning)();
    REPLACE bool activateTcLightning();

private:
    u8 _00c[0x020 - 0x00c];
    f32 m_internalSpeed;
    u8 _024[0x02c - 0x024];
    f32 m_hardSpeedLimit;
    u8 _030[0x038 - 0x030];
    Vec3<f32> m_up;
    Vec3<f32> m_rawUp;
    u8 _050[0x074 - 0x050];
    Vec3<f32> m_internalVelDir;
    u8 _080[0x1a8 - 0x080];
    s16 m_blinkTimer;
    u8 _1aa[0x294 - 0x1aa];
};
static_assert(sizeof(KartMove) == 0x294);

} // namespace Kart
