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
    Quat m_mainRot;
    Vec3 m_pos;

    // KartMove
    f32 m_internalSpeed;
    KartBoost m_boostState;

    MinifiedWheelPhysics m_wheelPhysics[4];

    KartItem m_item;
};

} // namespace Kart
