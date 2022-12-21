#include "HBMManager.hh"

namespace System {

bool HBMManager::isActive() const {
    return m_isActive;
}

HBMManager *HBMManager::Instance() {
    return s_instance;
}

} // namespace System
