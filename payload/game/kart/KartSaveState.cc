#include "KartSaveState.hh"

namespace Kart {

KartSaveState::KartSaveState(KartAccessor accessor, VehiclePhysics *physics, KartItem *item) {
    save(accessor, physics, item);
}

void KartSaveState::save(KartAccessor accessor, VehiclePhysics *physics, KartItem *item) {
    m_item = *item;
    m_physics = *physics;
    m_5c = *accessor.unk5c;
    m_sub = *accessor.sub;
    m_body = *accessor.body;
    m_move = *accessor.move;
    m_state = *accessor.state;
    m_action = *accessor.action;
    m_collide = *accessor.collide;

    for (u8 i = 0; i < accessor.settings->susCount; i++) {
        m_sus[i].base = *accessor.sus[i];
        m_sus[i].physics = *accessor.sus[i]->m_physics;
    }

    for (u8 i = 0; i < accessor.settings->tireCount; i++) {
        m_tire[i].tire = *accessor.tire[i];
        m_tire[i].physics = *accessor.tire[i]->m_wheelPhysics;
    }
}

void KartSaveState::reload(KartAccessor accessor, VehiclePhysics *physics, KartItem *item) {
    *item = m_item;
    *physics = m_physics;
    *accessor.sub = m_sub;
    *accessor.unk5c = m_5c;
    *accessor.body = m_body;
    *accessor.move = m_move;
    *accessor.state = m_state;
    *accessor.action = m_action;
    *accessor.collide = m_collide;

    for (u8 i = 0; i < accessor.settings->susCount; i++) {
        *accessor.sus[i] = m_sus[i].base;
        *accessor.sus[i]->m_physics = m_sus[i].physics;
    }

    for (u8 i = 0; i < accessor.settings->tireCount; i++) {
        *accessor.tire[i] = m_tire[i].tire;
        *accessor.tire[i]->m_wheelPhysics = m_tire[i].physics;
    }
}

} // namespace Kart
