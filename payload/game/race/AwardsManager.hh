#pragma once

#include "game/system/RaceConfig.hh"

namespace Race {

class AwardsManager {
public:
    void REPLACED(init)();
    REPLACE void init();

private:
    void initStand();
    void initPlayers(const System::RaceConfig::Scenario &awardsScenario,
            const u32 (&characterIds)[12]);
    void configure(bool isTeams, u32 playerCount, u32 localPlayerCount, u32 standPlayerCount);
    void dispatch(const u32 (&standCharacterIds)[12], const u32 (&audienceCharacterIds)[12]);

    u8 _000[0x474 - 0x000];
    bool m_isBikerModel[0x2a];
    u8 _49e[0x540 - 0x49e];
};
static_assert(sizeof(AwardsManager) == 0x540);

} // namespace Race
