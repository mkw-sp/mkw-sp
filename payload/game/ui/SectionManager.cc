#include "SectionManager.hh"

namespace UI {

Section *SectionManager::currentSection() {
    return m_currentSection;
}

GlobalContext *SectionManager::globalContext() {
    return m_globalContext;
}

SectionManager *SectionManager::Instance() {
    return s_instance;
}

} // namespace UI
