#include "AwardPage.hh"

#include "game/system/RaceConfig.hh"

#include <algorithm>
#include <array>
#include <numeric>

namespace UI {

AwardPage::AwardPage() = default;

AwardPage::~AwardPage() = default;

void AwardPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initConfig();
    const auto &awardsScenario = System::RaceConfig::Instance()->awardsScenario();
    bool isWin = awardsScenario.cameraMode >= 7 && awardsScenario.cameraMode <= 10;
    u32 maxTeamSize = awardsScenario.spMaxTeamSize;
    u32 teamCount = (m_playerCount + maxTeamSize - 1) / maxTeamSize;

    initChildren(2 + m_playerCount + (maxTeamSize == 1 ? 0 : teamCount));
    insertChild(0, &m_type, 0);
    insertChild(1, &m_congratulations, 0);
    for (size_t i = 0; i < m_playerCount; i++) {
        insertChild(2 + i, &m_items[i], 0);
    }
    for (size_t i = 0; i < (maxTeamSize == 1 ? 0 : teamCount); i++) {
        insertChild(2 + m_playerCount + i, &m_teams[i], 0);
    }

    m_type.load("award", isWin ? "AwardTypeWin" : "AwardTypeLose", "Type", nullptr);
    m_congratulations.load(isWin, awardsScenario.spMaxTeamSize >= 2 && awardsScenario.draw);
    for (size_t i = 0; i < m_playerCount; i++) {
        u32 positionId = (maxTeamSize == 6 ? 0 : 5 - maxTeamSize) * 12 + i;
        m_items[i].load(positionId, isWin, false);
    }
    for (size_t i = 0; i < (maxTeamSize == 1 ? 0 : teamCount); i++) {
        char variant[0x20];
        u32 positionId = (maxTeamSize == 6 ? 0 : 5 - maxTeamSize) * 6 + i;
        snprintf(variant, std::size(variant), "Pos%zu", positionId);
        m_teams[i].load("award", isWin ? "AwardTeamWin" : "AwardTeamLose", variant, nullptr);
        m_teams[i].setMessageAll(1032);
    }

    m_inputManager.setHandler(MenuInputManager::InputId::Front, &m_onFront, false);

    initType();
    initCongratulations();
    initItems();
    initTeams();
    initCup();
}

void AwardPage::initCongratulations() {
    const auto &awardsScenario = System::RaceConfig::Instance()->awardsScenario();
    u32 maxTeamSize = awardsScenario.spMaxTeamSize;
    if (maxTeamSize == 1) {
        REPLACED(initCongratulations)();
        return;
    }

    bool isWin = awardsScenario.cameraMode >= 7 && awardsScenario.cameraMode <= 10;
    if (isWin) {
        auto *winningPlayer = std::min_element(std::begin(awardsScenario.players),
                std::end(awardsScenario.players),
                [&](auto &p0, auto &p1) { return p0.rank < p1.rank; });
        assert(winningPlayer);
        u32 winningTeam = winningPlayer->spTeam;
        m_congratulations.setMessageAll(10284 + winningTeam);
    } else if (awardsScenario.draw) {
        m_congratulations.setMessageAll(1218);
    } else {
        m_congratulations.setMessageAll(1222);
    }
}

void AwardPage::initItems() {
    const auto &awardsScenario = System::RaceConfig::Instance()->awardsScenario();
    u32 maxTeamSize = awardsScenario.spMaxTeamSize;

    if (maxTeamSize == 1) {
        for (size_t i = 0; i < m_playerCount; i++) {
            m_items[awardsScenario.players[i].rank - 1].refresh(i, false, m_localPlayerCount > 1);
        }
        return;
    }

    std::array<u32, 12> playerIds{};
    std::iota(playerIds.begin(), playerIds.begin() + m_playerCount, 0);
    std::sort(playerIds.begin(), playerIds.begin() + m_playerCount, [&](auto i0, auto i1) {
        return awardsScenario.players[i0].score > awardsScenario.players[i1].score;
    });
    std::array<u32, 12> soloRanks{};
    for (size_t i = 0; i < m_playerCount; i++) {
        soloRanks[playerIds[i]] = i + 1;

        u32 score = awardsScenario.players[playerIds[i]].score;
        for (size_t j = i; j-- > 0 && awardsScenario.players[playerIds[j]].score == score;) {
            soloRanks[playerIds[i]] = j + 1;
        }
    }
    for (size_t i = 0; i < m_playerCount; i++) {
        m_items[awardsScenario.players[i].rank - 1].refresh(i, m_localPlayerCount > 1,
                soloRanks[i]);
    }
}

void AwardPage::initTeams() {
    const auto &awardsScenario = System::RaceConfig::Instance()->awardsScenario();
    u32 maxTeamSize = awardsScenario.spMaxTeamSize;
    u32 teamCount = (m_playerCount + maxTeamSize - 1) / maxTeamSize;

    if (maxTeamSize == 1) {
        return;
    }

    std::array<u32, 6> teamScores{};
    for (u32 playerId = 0; playerId < m_playerCount; playerId++) {
        u32 teamId = awardsScenario.players[playerId].spTeam;
        teamScores[teamId] += awardsScenario.players[playerId].score;
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
    for (u32 i = 0; i < teamCount; i++) {
        MessageInfo info{};
        info.intVals[0] = teamScores[teamIds[i]];
        info.messageIds[0] = 1301 + i;
        for (size_t j = i; j-- > 0 && teamScores[teamIds[j]] == teamScores[teamIds[i]];) {
            info.messageIds[0] = 1301 + j;
        }
        m_teams[i].setMessageAll(10290 + teamIds[i], &info);
    }
}

} // namespace UI
