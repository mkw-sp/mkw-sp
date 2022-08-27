#include "CtrlRaceResultTeamPoint.hh"

#include "game/ui/SectionManager.hh"
#include "game/util/Registry.hh"

namespace UI {

namespace GroupId {

enum {
    Position = 0,
};

} // namespace GroupId

CtrlRaceResultTeamPoint::CtrlRaceResultTeamPoint() = default;

CtrlRaceResultTeamPoint::~CtrlRaceResultTeamPoint() = default;

void CtrlRaceResultTeamPoint::load() {
    const char *groups[] = {
        "Position",
        "Position",
        nullptr,
        nullptr,
    };
    LayoutUIControl::load("result", "ResultTeamPoint", "ResultTeamPoint", groups);
}

void CtrlRaceResultTeamPoint::refresh(u32 colorId, u32 positionId) {
    m_animator.setAnimationInactive(GroupId::Position, 0, positionId);
    m_colorId = colorId;
}

void CtrlRaceResultTeamPoint::refresh(u32 score) {
    MessageInfo info{};
    info.intVals[0] = score;
    setMessage("point", 10275 + m_colorId, &info);
    setMessage("point_outline", 10281, &info);

    playSound(Sound::SoundId::SE_RSLT_POINT_UP, -1);
}

} // namespace UI
