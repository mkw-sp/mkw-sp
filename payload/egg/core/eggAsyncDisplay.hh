#pragma once

#include "egg/core/eggDisplay.hh"

namespace EGG {

class AsyncDisplay : public Display {
protected:
    void dummy_00() override;
    void dummy_04() override;

public:
    void REPLACED(beginFrame)();
    REPLACE void beginFrame() override;
    REPLACE void beginRender() override;
    void REPLACED(endRender)();
    REPLACE void endRender() override;
    u32 getTickPerFrame() /* override */;

private:
    u8 _28[0x88 - 0x28];
};
static_assert(sizeof(AsyncDisplay) == 0x88);

} // namespace EGG
