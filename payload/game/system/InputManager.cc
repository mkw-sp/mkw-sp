#include "InputManager.hh"

namespace System {

const RaceInputState &PadProxy::currentRaceInputState() const {
    return m_currentRaceInputState;
}

} // namespace System
