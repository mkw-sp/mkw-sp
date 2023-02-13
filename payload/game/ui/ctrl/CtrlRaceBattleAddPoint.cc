#include "CtrlRaceBattleAddPoint.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/ui/Page.hh"

namespace UI {

namespace GroupId {

enum {
    OnOff = 0,
};

} // namespace GroupId

namespace AnimId::OnOff {

enum {
    Off = 0,
    On = 1,
};

} // namespace AnimId::OnOff

CtrlRaceBattleAddPoint::CtrlRaceBattleAddPoint() = default;

CtrlRaceBattleAddPoint::~CtrlRaceBattleAddPoint() = default;

void CtrlRaceBattleAddPoint::calcSelf() {
    process();

    u32 playerId = getPlayerId();
    u16 score = System::RaceManager::Instance()->player(playerId)->battleScore();

    if (score == m_score) {
        if (static_cast<u32>(getPage()->frame() % 5) == m_frame &&
                m_animator.getAnimation(GroupId::OnOff) == AnimId::OnOff::Off) {
            if (m_scoreDiffCount != 0) {
                auto &raceScenario = System::RaceConfig::Instance()->raceScenario();

                MessageInfo info{};
                info.intVals[0] = m_scoreDiffs[0];
                u32 messageId;
                if (raceScenario.spMaxTeamSize >= 2) {
                    u32 playerId = getPlayerId();
                    messageId = 10399 + raceScenario.players[playerId].spTeam;
                } else {
                    messageId = 10414 + m_localPlayerId;
                }
                setMessage("point", messageId, &info);
                setMessage("point_outline", 10405, &info);

                for (u32 i = 0; i < m_scoreDiffCount - 1; i++) {
                    m_scoreDiffs[i] = m_scoreDiffs[i + 1];
                }
                m_scoreDiffCount--;

                m_animator.setAnimation(GroupId::OnOff, AnimId::OnOff::On, 0.0f);
            }

            m_animator.getGroup(GroupId::OnOff)->m_speed = std::min(m_scoreDiffCount + 1.0f, 5.0f);
            setVisible(m_animator.getAnimation(GroupId::OnOff) != AnimId::OnOff::Off);
        }
    } else {
        m_scoreDiffCount = std::min(m_scoreDiffCount, std::size(m_scoreDiffs));
        m_scoreDiffs[m_scoreDiffCount++] = score - m_score;
        m_score = score;
        m_frame = getPage()->frame() % 5;
    }
}

} // namespace UI
