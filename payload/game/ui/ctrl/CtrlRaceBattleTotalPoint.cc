#include "CtrlRaceBattleTotalPoint.hh"

#include "game/sound/SceneSoundManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

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
    for (u32 i = 0; i < 6; i++) {
        MessageInfo info{};
        info.intVals[0] = 0;
        char paneName[0x20];
        snprintf(paneName, std::size(paneName), "point_%u", i);
        setMessage(paneName, 10275 + i, &info);
        snprintf(paneName, std::size(paneName), "point_outline_%u", i);
        setMessage(paneName, 10281, &info);

        m_animator.setAnimation(i, AnimId::On, 0.0f);

        m_scores[i] = 0;

        snprintf(paneName, std::size(paneName), "null_%u", i);
        setPaneVisible(paneName, i < m_teamCount);
    }
    LayoutUIControl::init();
}

void CtrlRaceBattleTotalPoint::calcSelf() {
    process();

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    for (u32 i = 0; i < m_teamCount; i++) {
        u32 score = 0;
        for (u32 j = 0; j < m_playerCount; j++) {
            if (raceScenario.players[j].spTeam != i) {
                continue;
            }

            score += System::RaceManager::Instance()->player(j)->battleScore();
        }

        if (score == m_scores[i]) {
            continue;
        }

        MessageInfo info{};
        info.intVals[0] = score;
        char paneName[0x20];
        snprintf(paneName, std::size(paneName), "point_%u", i);
        setMessage(paneName, 10275 + i, &info);
        snprintf(paneName, std::size(paneName), "point_outline_%u", i);
        setMessage(paneName, 10281, &info);

        m_animator.setAnimation(i, AnimId::Off, 0.0f);

        f32 pan = 2.0f * i / (m_teamCount - 1);
        if (score > m_scores[i]) {
            if (playSound(Sound::SoundId::SE_RC_BTL_POINT_UP_RED, -1)) {
                Sound::SceneSoundManager::Handle().setPan(pan);
            }
        } else {
            if (playSound(Sound::SoundId::SE_RC_BTL_POINT_DOWN_RED, -1)) {
                Sound::SceneSoundManager::Handle().setPan(pan);
            }
        }

        m_scores[i] = score;
    }
}

void CtrlRaceBattleTotalPoint::load() {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();

    m_playerCount = raceScenario.playerCount;
    u32 maxTeamSize = raceScenario.spMaxTeamSize;
    m_teamCount = (m_playerCount + maxTeamSize - 1) / maxTeamSize;

    u32 screenCount = raceScenario.screenCount == 3 ? 4 : raceScenario.screenCount;
    char variant[0x20];
    snprintf(variant, std::size(variant), "total_point_%u_%u", screenCount, m_teamCount);
    const char *groups[] = {
        "null_0",
        "off",
        "on",
        "stop",
        nullptr,
        "null_1",
        "off",
        "on",
        "stop",
        nullptr,
        "null_2",
        "off",
        "on",
        "stop",
        nullptr,
        "null_3",
        "off",
        "on",
        "stop",
        nullptr,
        "null_4",
        "off",
        "on",
        "stop",
        nullptr,
        "null_5",
        "off",
        "on",
        "stop",
        nullptr,
        nullptr,
    };
    LayoutUIControl::load("game_image", "battle_total_point", variant, groups);
}

} // namespace UI
