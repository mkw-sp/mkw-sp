#include "eggSystem.hh"

namespace EGG {

void *TSystem::mem1ArenaLo() const {
    return m_mem1ArenaLo;
}

void *TSystem::mem1ArenaHi() const {
    return m_mem1ArenaHi;
}

void *TSystem::mem2ArenaLo() const {
    return m_mem2ArenaLo;
}

void *TSystem::mem2ArenaHi() const {
    return m_mem2ArenaHi;
}

Heap *TSystem::eggRootMEM1() const {
    return m_eggRootMEM1;
}

Heap *TSystem::eggRootMEM2() const {
    return m_eggRootMEM2;
}

Heap *TSystem::eggRootDebug() const {
    return m_eggRootDebug;
}

Heap *TSystem::eggRootSystem() const {
    return m_eggRootSystem;
}

TSystem &TSystem::Instance() {
    return s_instance;
}

} // namespace EGG
