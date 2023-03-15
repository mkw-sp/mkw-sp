#include "DriverManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

#include <algorithm>
#include <numeric>

namespace Race {

DriverManager *DriverManager::Instance() {
    return s_instance;
}

DriverManager::Outcome DriverManager::GetTeamPlayerOutcome(u32 playerId) {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    auto *raceManager = System::RaceManager::Instance();
    u32 playerCount = raceScenario.playerCount;
    u32 maxTeamSize = raceScenario.spMaxTeamSize;

    if (maxTeamSize < 2) {
        u32 rank = raceManager->player(playerId)->rank();
        if (rank == 1) {
            return Outcome::Best;
        } else if (rank <= std::min<u32>((playerCount + 1) / 2, 5)) {
            return Outcome::Good;
        } else {
            return Outcome::Bad;
        }
    }

    u32 teamCount = (playerCount + maxTeamSize - 1) / maxTeamSize;
    std::array<u32, 6> teamScores{};
    for (u32 i = 0; i < playerCount; i++) {
        u32 teamId = raceScenario.players[i].spTeam;
        teamScores[teamId] += raceManager->player(i)->battleScore();
    }
    std::array<u32, 6> teamIds{};
    std::iota(teamIds.begin(), teamIds.begin() + teamCount, 0);
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104165
    PRAGMA("GCC diagnostic push")
    PRAGMA("GCC diagnostic ignored \"-Warray-bounds\"")
    std::sort(teamIds.begin(), teamIds.begin() + teamCount,
            [&](auto i0, auto i1) { return teamScores[i0] > teamScores[i1]; });
    PRAGMA("GCC diagnostic pop")
    std::array<u32, 6> teamIndices{};
    for (u32 i = 0; i < teamCount; i++) {
        teamIndices[teamIds[i]] = i;
    }

    u32 draw;
    for (draw = 0; static_cast<u32>(draw + 1) < teamCount &&
            teamScores[teamIds[draw]] == teamScores[teamIds[draw + 1]];
            draw++) {}

    if (teamIndices[raceScenario.players[playerId].spTeam] >= draw + 1) {
        return Outcome::Bad;
    }

    if (draw > 0) {
        return Outcome::Draw;
    }

    u32 score = raceManager->player(playerId)->battleScore();
    for (u32 i = 0; i < playerCount; i++) {
        if (raceManager->player(i)->battleScore() > score) {
            return Outcome::Good;
        }
    }

    return Outcome::Best;
}

} // namespace Race
