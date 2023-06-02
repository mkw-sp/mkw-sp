#pragma once

#include <egg/core/eggSystem.hh>

namespace System {

class RkSystem : public EGG::TSystem {
public:
    RkSystem();
    virtual ~RkSystem();

private:
    REPLACE void initialize() override;
    void REPLACED(initialize)();
};

} // namespace System
