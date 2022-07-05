#include "KartObjectProxy.hh"

namespace Kart {

KartState *KartObjectProxy::getKartState() {
    return m_accessor->state;
}

} // namespace Kart
