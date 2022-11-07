#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
public:
    f32 hardSpeedLimit() const;
    const Vec3 *internalVelDir() const;
    void ActivateMega();
    void REPLACED(calcBlink)();
    REPLACE void calcBlink();
    void REPLACED(ThunderActive)(int timer, int param_3, int param_4);
    REPLACE bool ThunderActive(int timer, int param_3, int param_4);

private:
    u8 _00c[0x020 - 0x00c];
    f32 m_internalSpeed;
    u8 _024[0x02c - 0x024];
    f32 m_hardSpeedLimit;
    u8 _030[0x074 - 0x030];
    Vec3 m_internalVelDir;
    u8 _080[0x1a8 - 0x080];
    s16 m_blinkTimer;
    u8 _1aa[0x294 - 0x1aa];
};
static_assert(sizeof(KartMove) == 0x294);

} // namespace Kart
