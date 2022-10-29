#pragma once

#include "egg/core/eggXfbManager.hh"

namespace EGG {

class TSystem {
public:
    void *mem1ArenaLo() const;
    void *mem1ArenaHi() const;
    void *mem2ArenaLo() const;
    void *mem2ArenaHi() const;
    XfbManager *xfbManager();

    static TSystem *Instance();

private:
    u8 _00[0x04 - 0x00];
    void *m_mem1ArenaLo;
    void *m_mem1ArenaHi;
    void *m_mem2ArenaLo;
    void *m_mem2ArenaHi;
    u8 _14[0x48 - 0x14];
    XfbManager *m_xfbManager;
    // ...

    static TSystem s_instance;
};

} // namespace EGG
