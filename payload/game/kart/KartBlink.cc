#include "KartBlink.hh"

#include <sp/ThumbnailManager.hh>

namespace Kart {

void KartBlink::calc(bool stop) {
    if (SP::ThumbnailManager::IsActive()) {
        m_visible = false;
        m_frame = 0;
    }

    REPLACED(calc)(stop);
}

} // namespace Kart
