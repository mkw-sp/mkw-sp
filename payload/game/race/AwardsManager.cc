#include "AwardsManager.hh"

#include <algorithm>
#include <numeric>

namespace Race {

void AwardsManager::init() {
    const auto &awardsScenario = System::RaceConfig::Instance()->awardsScenario();
    if (awardsScenario.spMaxTeamSize == 1) {
        REPLACED(init)();
        return;
    }

    initStand();
    u32 characterIds[12];
    initPlayers(awardsScenario, characterIds);
    u32 playerCount = 0;
    u32 localPlayerCount = 0;
    for (u32 playerId = 0; playerId < 12; playerId++) {
        auto playerType = awardsScenario.players[playerId].type;
        if (playerType == System::RaceConfig::Player::Type::Local) {
            localPlayerCount++;
        } else if (playerType != System::RaceConfig::Player::Type::CPU) {
            continue;
        }
        playerCount++;

        if (awardsScenario.players[playerId].vehicleId >= 0x12) {
            switch (characterIds[playerId]) {
            case 0xf:  // Daisy
            case 0x10: // Peach
            case 0x17: // Rosalina
                m_isBikerModel[characterIds[playerId]] = true;
                break;
            }
        }
    }
    std::array<u32, 12> playerIds{};
    std::iota(playerIds.begin(), playerIds.begin() + playerCount, 0);
    std::sort(playerIds.begin(), playerIds.begin() + playerCount, [&](auto i0, auto i1) {
        return awardsScenario.players[i0].rank > awardsScenario.players[i1].rank;
    });
    u32 standPlayerCount = 0;
    u32 standCharacterIds[12];
    std::fill(std::begin(standCharacterIds), std::end(standCharacterIds), 0x2a);
    u32 audiencePlayerCount = 0;
    u32 audienceCharacterIds[12];
    std::fill(std::begin(audienceCharacterIds), std::end(audienceCharacterIds), 0x2a);
    for (u32 playerId : playerIds) {
        if (awardsScenario.players[playerId].rank <= awardsScenario.spMaxTeamSize) {
            standCharacterIds[standPlayerCount++] = characterIds[playerId];
        } else {
            audienceCharacterIds[audiencePlayerCount++] = characterIds[playerId];
        }
    }
    configure(true, playerCount, localPlayerCount, standPlayerCount);
    dispatch(standCharacterIds, audienceCharacterIds);
}

} // namespace Race
