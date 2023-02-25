#include "KartSaveState.hh"

namespace Kart {

Kart::KartSaveState* kartSaveState = nullptr;

void KartSaveState::save(KartAccessor accessor, VehiclePhysics* physics) {
    m_physics = *physics;
    m_5c = *accessor.unk5c;
    m_sub = *accessor.sub;
    m_body = *accessor.body;
    m_move = *accessor.move;
    m_state = *accessor.state;
    m_action = *accessor.action;
    m_collide = *accessor.collide;

    auto susCount = accessor.settings->wheelCount0;
    for (u8 i = 0; i < susCount; i++) {
        m_sus[i].base = *accessor.sus[i];
        m_sus[i].physics = *accessor.sus[i]->m_physics;
    }

    auto tireCount = accessor.settings->wheelCount1;
    for (u8 i = 0; i < tireCount; i++) {
        m_tire[i].tire = *accessor.tire[i];
        m_tire[i].physics = *accessor.tire[i]->m_wheelPhysics;
    }
}

void KartSaveState::reload(KartAccessor accessor, VehiclePhysics* physics) {
    *physics = m_physics;
    *accessor.sub = m_sub;
    *accessor.unk5c = m_5c;
    *accessor.body = m_body;
    *accessor.move = m_move;
    *accessor.state = m_state;
    *accessor.action = m_action;
    *accessor.collide = m_collide;

    auto susCount = accessor.settings->wheelCount0;
    for (u8 i = 0; i < susCount; i++) {
        *accessor.sus[i] = m_sus[i].base;
        *accessor.sus[i]->m_physics = m_sus[i].physics;
    }

    auto tireCount = accessor.settings->wheelCount1;
    for (u8 i = 0; i < tireCount; i++) {
        *accessor.tire[i] = m_tire[i].tire;
        *accessor.tire[i]->m_wheelPhysics = m_tire[i].physics;
    }
}

} // namespace Kart

