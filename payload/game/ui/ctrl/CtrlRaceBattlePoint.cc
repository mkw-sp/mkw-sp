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
    u32 playerId = getPlayerId();

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    u32 teamId = raceScenario.players[playerId].spTeam;

    MessageInfo info{};
    info.intVals[0] = score;
    setMessage("point", 10275 + teamId, &info);
    setMessage("point_outline", 10281, &info);

    m_score = score;
}

} // namespace UI
