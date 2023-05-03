#pragma once

#include <egg/core/eggSystem.hh>

namespace System {

class RkSystem : public EGG::TSystem {
private:
    REPLACE void initialize();
    void REPLACED(initialize)();
};

} // namespace System
