#include "KartObject.hh"

#include "game/kart/KartRollback.hh"

#include <sp/cs/RoomClient.hh>

namespace Kart {

void KartObject::init() {
    REPLACED(init)();

    if (SP::RoomClient::Instance()) {
        m_accessor.rollback = new KartRollback;
    }
}

void KartObject::calcEarly() {
    if (SP::RoomClient::Instance()) {
        m_accessor.rollback->calcEarly();
    }

    m_sub->calcEarly();
}

void KartObject::calcLate() {
    REPLACED(calcLate)();

    if (SP::RoomClient::Instance()) {
        m_accessor.rollback->calcLate();
    }
}

} // namespace Kart
