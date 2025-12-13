#pragma once

#include "game/kart/KartBoost.hh"
#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
    friend class KartObjectProxy;
    friend class KartRollback;
    friend class KartSaveState;

public:
    f32 hardSpeedLimit() const;
    const Vec3 *internalVelDir() const;

    KartMove();
    virtual ~KartMove();
    virtual void dt(s32 type);
    virtual void *createComponents();
    virtual void *setTurnParams();
    void REPLACED(init)(u32 r4, u32 r5);
    REPLACE virtual void init(u32 r4, u32 r5);

    REPLACE void applyBulletSpeedLimit();
    REPLACE void revertBulletSpeedLimit();
    void activateBoost(u8 type, s16 duration);
    void activateMega();
    void REPLACED(calcBlink)();
    REPLACE void calcBlink();
    bool REPLACED(activateTcLightning)();
    REPLACE bool activateTcLightning();

private:
    u8 _010[0x020 - 0x010];
    f32 m_internalSpeed;
    u8 _024[0x02c - 0x024];
    f32 m_hardSpeedLimit;
    u8 _030[0x038 - 0x030];
    Vec3 m_up;
    Vec3 m_rawUp;
    u8 _050[0x05c - 0x050];
    Vec3 m_dir;
    u8 _068[0x074 - 0x068];
    Vec3 m_internalVelDir;
    u8 _080[0x108 - 0x080];
    KartBoost m_boost;
    u8 _12c[0x1a8 - 0x12c];
    s16 m_timeBeforeBlinkEnd;
    u8 _1aa[0x234 - 0x1aa];
    u16 m_timeInRespawn;
    u8 _236[0x294 - 0x236];
};
static_assert(sizeof(KartMove) == 0x294);

} // namespace Kart
