#pragma once

#include "egg/core/eggHeap.hh"
#include "egg/core/eggXfbManager.hh"

namespace EGG {

class TSystem {
public:
    void *mem1ArenaLo() const;
    void *mem1ArenaHi() const;
    void *mem2ArenaLo() const;
    void *mem2ArenaHi() const;
    Heap *eggRootMEM1() const;
    Heap *eggRootMEM2() const;
    Heap *eggRootDebug() const;
    Heap *eggRootSystem() const;
    XfbManager *xfbManager();

    static TSystem &Instance();

private:
    u8 _00[0x04 - 0x00];
    void *m_mem1ArenaLo;
    void *m_mem1ArenaHi;
    void *m_mem2ArenaLo;
    void *m_mem2ArenaHi;
    u8 _14[0x18 - 0x14];
    Heap *m_eggRootMEM1;
    Heap *m_eggRootMEM2;
    Heap *m_eggRootDebug;
    Heap *m_eggRootSystem;
    u8 _28[0x48 - 0x28];
    XfbManager *m_xfbManager;
    // ...

    static TSystem s_instance;
};

} // namespace EGG
