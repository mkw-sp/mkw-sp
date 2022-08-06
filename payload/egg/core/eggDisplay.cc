#include "eggDisplay.hh"

#include <sp/PerfOverlay.hh>

namespace EGG {

void Display::endFrame() {
    SP::PerfOverlay::MeasureEndCalc();

    GXDrawDone();
}

} // namespace EGG
