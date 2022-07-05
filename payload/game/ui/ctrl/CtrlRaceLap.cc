#include "CtrlRaceLap.hh"

namespace UI {

void CtrlRaceLap::calcSelf() {
    calcLabelVisibility("lap_text");

    REPLACED(calcSelf)();
}

} // namespace UI
