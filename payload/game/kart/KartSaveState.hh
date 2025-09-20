// Handles storing and restoring as much state about the kart as needed
// to allow for faster dolphin-like save states on console for practicing.
//
// This class is only for low level management, further functionality should
// most likely be added to SP::SaveStateManager.

#pragma once

#include "KartBoost.hh"
#include "KartMove.hh"
#include "KartObjectManager.hh"
#include "KartTire.hh"
#include "VehiclePhysics.hh"

extern "C" {
#include "game/item/ItemDirector.h"
}

namespace Kart {

// KartBoost's constructor cannot be patched to replace
// the vtable, and therefore define a GCC destructor
// as it is inlined in KartMove.
struct PODKartBoost {
    s16 m_timesBeforeEnd[6];
    u16 m_types;
    f32 m_boostMultipler;
    f32 m_boostAcceleration;
    f32 m_1c;
    f32 m_boostSpeedLimit;
};

struct MinifiedWheelPhysics {
    Vec3 m_realPos;
    Vec3 m_lastPos;
    Vec3 m_lastPosDiff;
};

class KartSaveState {
public:
    KartSaveState(KartAccessor accessor, VehiclePhysics *physics, KartItem *item);

    void save(KartAccessor accessor, VehiclePhysics *physics, KartItem *item);
    void reload(KartAccessor accessor, VehiclePhysics *physics, KartItem *item);

private:
    // VehiclePhysics
    Vec3 m_externalVel;
    Vec3 m_internalVel;
    bool m_inBullet;
    Quat m_mainRot;
    Vec3 m_pos;

    // KartMove
    f32 m_internalSpeed;
    PODKartBoost m_boostState;
    Vec3 m_up;
    Vec3 m_dir;

    // KartState
    u32 m_airtime;

    MinifiedWheelPhysics m_wheelPhysics[4];

    KartItem m_item;
};

} // namespace Kart
