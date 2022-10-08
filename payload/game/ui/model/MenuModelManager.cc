#include "MenuModelManager.hh"

namespace UI {

DriverModelHandle *DriverModelManager::handle(size_t index) {
    return &m_handles[index];
}

f32 DriverModelManager::getDelay() const {
    return m_delay;
}

DriverModelManager *MenuModelManager::driverModelManager() {
    return m_driverModelManager;
}

MenuModelManager *MenuModelManager::Instance() {
    return s_instance;
}

} // namespace UI
