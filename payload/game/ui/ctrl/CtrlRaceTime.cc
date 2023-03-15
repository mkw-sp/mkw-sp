#include "CtrlRaceTime.hh"

namespace UI {

void CtrlRaceTime::calcSelf() {
    calcLabelVisibility("set_p");

    REPLACED(calcSelf)();
}

s8 CtrlRaceTime::getPlayerId() {
    return CtrlRaceBase::getPlayerId();
}

} // namespace UI
