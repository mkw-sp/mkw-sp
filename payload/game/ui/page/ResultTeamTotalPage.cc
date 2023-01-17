#include "ResultTeamTotalPage.hh"

#include "game/system/RaceConfig.hh"

#include <algorithm>
#include <numeric>

namespace UI {

ResultTeamTotalPage::ResultTeamTotalPage() = default;

ResultTeamTotalPage::~ResultTeamTotalPage() = default;

PageId ResultTeamTotalPage::getReplacement() {
    auto raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.isBattle()) {
        return PageId::AfterBtMenu;
    } else {
        return PageId::AfterVsMenu;
    }
}

void ResultTeamTotalPage::onInit() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    u32 maxTeamSize = raceScenario.spMaxTeamSize;
    if (maxTeamSize == 1) {
        initChildren(0);
        return;
    }
    u32 playerCount = raceScenario.playerCount;
    u32 teamCount = (playerCount + maxTeamSize - 1) / maxTeamSize;

    initChildren(playerCount + teamCount);

    for (size_t i = 0; i < playerCount; i++) {
        insertChild(i, &m_controls[i], 0);
    }
    for (size_t i = 0; i < teamCount; i++) {
        insertChild(std::size(m_controls) + i, &m_pointControls[i], 0);
    }

    for (size_t i = 0; i < playerCount; i++) {
        m_controls[i].load();
    }
    for (size_t i = 0; i < teamCount; i++) {
        m_pointControls[i].load();
    }
}

void ResultTeamTotalPage::onActivate() {
    ResultPage::onActivate();

    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    u32 playerCount = raceScenario.playerCount;
    u32 maxTeamSize = raceScenario.spMaxTeamSize;
    u32 teamCount = (playerCount + maxTeamSize - 1) / maxTeamSize;
    std::array<u32, 12> playerIds{};
    std::iota(playerIds.begin(), playerIds.begin() + playerCount, 0);
    std::sort(playerIds.begin(), playerIds.begin() + playerCount, [&](auto i0, auto i1) {
        return menuScenario.players[i0].score > menuScenario.players[i1].score;
    });
    std::array<u32, 6> teamScores{};
    for (u32 playerId = 0; playerId < playerCount; playerId++) {
        u32 teamId = raceScenario.players[playerId].spTeam;
        teamScores[teamId] += menuScenario.players[playerId].score;
    }
    std::array<u32, 6> teamIds{};
    std::iota(teamIds.begin(), teamIds.begin() + teamCount, 0);
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104165
    PRAGMA("GCC diagnostic push")
    PRAGMA("GCC diagnostic ignored \"-Warray-bounds\"")
    std::sort(teamIds.begin(), teamIds.begin() + teamCount, [&](auto i0, auto i1) {
        return teamScores[i0] > teamScores[i1];
    });
    PRAGMA("GCC diagnostic pop")
    std::array<u32, 6> teamIndices{};
    for (u32 i = 0; i < teamCount; i++) {
        teamIndices[teamIds[i]] = i;
    }
    std::array<u32, 6> teamSizes{};
    for (u32 playerIndex = 0; playerIndex < playerCount; playerIndex++) {
        u32 playerId = playerIds[playerIndex];
        u32 characterId = raceScenario.players[playerId].characterId;
        u32 teamId = raceScenario.players[playerId].spTeam;
        u32 rank = teamIndices[teamId] * maxTeamSize + teamSizes[teamId]++;
        u32 positionId = (maxTeamSize == 6 ? 0 : 5 - maxTeamSize) * 12 + rank;
        m_controls[playerId].refresh(playerId, characterId, teamId, positionId);

        menuScenario.players[playerId].rank = rank + 1;
    }
    for (u32 teamIndex = 0; teamIndex < teamCount; teamIndex++) {
        u32 teamId = teamIds[teamIndex];
        u32 positionId = (maxTeamSize == 6 ? 0 : 5 - maxTeamSize) * 6 + teamIndex;
        m_pointControls[teamId].refresh(teamId, positionId);
    }

    for (menuScenario.draw = 0; static_cast<u32>(menuScenario.draw + 1) < teamCount &&
            teamScores[teamIds[menuScenario.draw]] == teamScores[teamIds[menuScenario.draw + 1]];
            menuScenario.draw++) {}

    m_playerScores = {};
    m_teamScores = {};
    for (u32 playerId = 0; playerId < playerCount; playerId++) {
        m_playerScores[playerId] = raceScenario.players[playerId].prevScore;
        u32 teamId = raceScenario.players[playerId].spTeam;
        m_teamScores[teamId] += raceScenario.players[playerId].prevScore;
    }
    m_frame = 0;
    m_isBusy = true;
}

void ResultTeamTotalPage::beforeCalc() {
    if (m_frame++ % 2 != 0) {
        return;
    }

    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    const auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    u32 playerCount = raceScenario.playerCount;
    u32 maxTeamSize = raceScenario.spMaxTeamSize;
    u32 teamCount = (playerCount + maxTeamSize - 1) / maxTeamSize;
    std::array<u32, 6> teamScores{};
    for (u32 playerId = 0; playerId < playerCount; playerId++) {
        if (m_playerScores[playerId] <= menuScenario.players[playerId].score) {
            m_controls[playerId].refresh(m_playerScores[playerId]);
            m_playerScores[playerId]++;
        }
        teamScores[raceScenario.players[playerId].spTeam] += m_playerScores[playerId] - 1;
    }
    m_isBusy = false;
    for (u32 teamId = 0; teamId < teamCount; teamId++) {
        if (m_teamScores[teamId] <= teamScores[teamId]) {
            m_pointControls[teamId].refresh(teamScores[teamId]);
            m_teamScores[teamId] = teamScores[teamId] + 1;
            m_isBusy = true;
        }
    }
}

bool ResultTeamTotalPage::isBusy() {
    return m_isBusy;
}

void ResultTeamTotalPage::vf_68() {}

} // namespace UI
