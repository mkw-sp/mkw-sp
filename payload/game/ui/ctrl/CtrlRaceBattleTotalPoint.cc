#include "CtrlRaceBattleTotalPoint.hh"

#include "game/sound/SceneSoundManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

#include <algorithm>
#include <cstdio>
#include <numeric>

namespace UI {

namespace AnimId {

enum {
    Off = 0,
    On = 1,
    Stop = 2,
};

} // namespace AnimId

CtrlRaceBattleTotalPoint::CtrlRaceBattleTotalPoint() = default;

CtrlRaceBattleTotalPoint::~CtrlRaceBattleTotalPoint() = default;

void CtrlRaceBattleTotalPoint::init() {
    std::fill(m_teamScores.begin(), m_teamScores.begin() + m_teamCount, 0);
    std::iota(m_teamIds.begin(), m_teamIds.begin() + m_teamCount, 0);
    std::fill(m_scores.begin(), m_scores.begin() + m_teamCount, 0);

    for (u32 i = 0; i < 6; i++) {
        MessageInfo info{};
        info.intVals[0] = 0;
        char paneName[0x20];
        snprintf(paneName, std::size(paneName), "point_%u", i);
        setMessage(paneName, 10275 + i, &info);
        snprintf(paneName, std::size(paneName), "point_outline_%u", i);
        setMessage(paneName, 10281, &info);

        m_animator.setAnimation(i, AnimId::On, 0.0f);

        snprintf(paneName, std::size(paneName), "null_%u", i);
        setPaneVisible(paneName, i < m_teamCount);
    }
    LayoutUIControl::init();
}

void CtrlRaceBattleTotalPoint::calcSelf() {
    process();

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    std::array<u32, 6> teamScores{};
    for (u32 i = 0; i < raceScenario.playerCount; i++) {
        u32 teamId = raceScenario.players[i].spTeam;
        teamScores[teamId] += System::RaceManager::Instance()->player(i)->battleScore();
    }

    std::array<u32, 6> teamIds = m_teamIds;
    // std::stable_sort allocates memory so we just use insertion sort here
    for (u32 i = 0; i < m_teamCount; i++) {
        for (u32 j = i; j > 0 && teamScores[teamIds[j - 1]] < teamScores[teamIds[j]]; j--) {
            std::swap(teamIds[j - 1], teamIds[j]);
        }
    }

    std::array<u32, 6> scores{};
    for (u32 i = 0; i < m_teamCount; i++) {
        scores[i] = teamScores[teamIds[i]];
    }

    for (u32 i = 0; i < m_teamCount; i++) {
        if (teamIds[i] == m_teamIds[i] && scores[i] == m_scores[i]) {
            continue;
        }

        MessageInfo info{};
        info.intVals[0] = scores[i];
        char paneName[0x20];
        snprintf(paneName, std::size(paneName), "point_%u", i);
        setMessage(paneName, 10275 + teamIds[i], &info);
        snprintf(paneName, std::size(paneName), "point_outline_%u", i);
        setMessage(paneName, 10281, &info);

        m_animator.setAnimation(i, AnimId::Off, 0.0f);

        f32 pan = 2.0f * i / (m_teamCount - 1);
        if (teamScores[teamIds[i]] > m_teamScores[teamIds[i]]) {
            if (playSound(Sound::SoundId::SE_RC_BTL_POINT_UP_RED, -1)) {
                Sound::SceneSoundManager::Handle().setPan(pan);
            }
        } else if (teamScores[teamIds[i]] < m_teamScores[teamIds[i]]) {
            if (playSound(Sound::SoundId::SE_RC_BTL_POINT_DOWN_RED, -1)) {
                Sound::SceneSoundManager::Handle().setPan(pan);
            }
        }
    }

    m_teamScores = teamScores;
    m_teamIds = teamIds;
    m_scores = scores;
}

void CtrlRaceBattleTotalPoint::load() {
    char variant[0x20];
    // clang-format off
    const char *groups[] = {
        "null_0", "off", "on", "stop", nullptr,
        "null_1", "off", "on", "stop", nullptr,
        "null_2", "off", "on", "stop", nullptr,
        "null_3", "off", "on", "stop", nullptr,
        "null_4", "off", "on", "stop", nullptr,
        "null_5", "off", "on", "stop", nullptr,
        nullptr,
    };
    // clang-format on

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();

    u32 maxTeamSize = raceScenario.spMaxTeamSize;
    m_teamCount = (raceScenario.playerCount + maxTeamSize - 1) / maxTeamSize;

    u32 screenCount = raceScenario.screenCount == 3 ? 4 : raceScenario.screenCount;
    snprintf(variant, std::size(variant), "total_point_%u_%u", screenCount, m_teamCount);

    LayoutUIControl::load("game_image", "battle_total_point", variant, groups);
}

} // namespace UI
