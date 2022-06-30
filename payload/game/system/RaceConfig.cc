#include "RaceConfig.hh"

namespace System {

RaceConfig::Scenario &RaceConfig::raceScenario() {
    return m_raceScenario;
}

RaceConfig::Scenario &RaceConfig::menuScenario() {
    return m_menuScenario;
}

RaceConfig::Scenario &RaceConfig::awardsScenario() {
    return m_awardsScenario;
}

u8 (&RaceConfig::ghostBuffers())[2][11][0x2800] {
    return m_ghostBuffers;
}

RaceConfig *RaceConfig::Instance() {
    return s_instance;
}

} // namespace System
