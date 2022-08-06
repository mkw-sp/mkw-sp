#include "eggAsyncDisplay.hh"

#include <sp/PerfOverlay.hh>

namespace EGG {

void AsyncDisplay::beginFrame() {
    REPLACED(beginFrame)();

    SP::PerfOverlay::MeasureBeginFrame(getTickPerFrame());
}

void AsyncDisplay::beginRender() {
    GXDrawDone();

    SP::PerfOverlay::MeasureBeginRender();
}

void AsyncDisplay::endRender() {
    SP::PerfOverlay::Draw();
    SP::PerfOverlay::MeasureEndRender();

    REPLACED(endRender)();

    SP::PerfOverlay::MeasureBeginCalc();
}

} // namespace EGG
