#include "CtrlRaceBattlePoint.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

namespace UI {

CtrlRaceBattlePoint::CtrlRaceBattlePoint() = default;

CtrlRaceBattlePoint::~CtrlRaceBattlePoint() = default;

void CtrlRaceBattlePoint::init() {
    u32 playerId = getPlayerId();
    u16 score = System::RaceManager::Instance()->player(playerId)->battleScore();
    refresh(score);

    LayoutUIControl::init();
}

void CtrlRaceBattlePoint::calcSelf() {
    process();

    u32 playerId = getPlayerId();
    u16 score = System::RaceManager::Instance()->player(playerId)->battleScore();
    if (score == m_score) {
        return;
    }
    refresh(score);
}

void CtrlRaceBattlePoint::refresh(u32 score) {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();

    MessageInfo info{};
    info.intVals[0] = score;
    u32 messageId;
    if (raceScenario.spMaxTeamSize >= 2) {
        u32 playerId = getPlayerId();
        messageId = 10275 + raceScenario.players[playerId].spTeam;
    } else {
        messageId = 10410 + m_localPlayerId;
    }
    setMessage("point", messageId, &info);
    setMessage("point_outline", 10281, &info);

    m_score = score;
}

} // namespace UI
