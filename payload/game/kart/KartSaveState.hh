// Handles storing and restoring as much state about the kart as possible
// to allow for faster dolphin-like save states on console for practicing.
//
// This class is only for low level management, further functionality should
// most likely be added to SP::SaveStateManager.

#pragma once

#include "Kart5c.hh"
#include "KartSus.hh"
#include "KartSub.hh"
#include "KartBody.hh"
#include "KartMove.hh"
#include "KartTire.hh"
#include "KartState.hh"
#include "KartAction.hh"
#include "KartCollide.hh"
#include "KartSettings.hh"
#include "VehiclePhysics.hh"

namespace Kart {

struct KartSusFlat {
    KartSus base;
    KartSusPhysics physics;
};

struct KartTireFlat {
    KartTire tire;
    WheelPhysics physics;
};

class KartSaveState {
public:
    KartSaveState() = default;

    void save(KartAccessor accessor, VehiclePhysics* physics);
    void reload(KartAccessor accessor, VehiclePhysics* physics);
private:
    Kart5c m_5c;
    KartSub m_sub;
    KartBody m_body;
    KartMove m_move;
    KartState m_state;
    KartAction m_action;
    KartSusFlat m_sus[4];
    KartTireFlat m_tire[4];
    KartCollide m_collide;
    VehiclePhysics m_physics;
};

} // namespace Kart
