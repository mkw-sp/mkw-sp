#include "CtrlRaceResultTeam.hh"

#include "game/ui/SectionManager.hh"
#include "game/ui/TeamColors.hh"
#include "game/util/Registry.hh"

namespace UI {

namespace GroupId {

enum {
    Loop = 0,
    Select = 1,
    Select2 = 2,
    Position = 3,
};

} // namespace GroupId

CtrlRaceResultTeam::CtrlRaceResultTeam() = default;

CtrlRaceResultTeam::~CtrlRaceResultTeam() = default;

void CtrlRaceResultTeam::load() {
    const char *groups[] = {
        "Loop",
        "Loop",
        nullptr,
        "Select",
        "SelectOn",
        "SelectOff",
        nullptr,
        "Select2",
        "Select2On",
        "Select2Off",
        nullptr,
        "Position",
        "Position",
        nullptr,
        nullptr,
    };
    LayoutUIControl::load("result", "ResultTeam", "ResultTeam", groups);

    setSoundIds(Sound::SoundId::SE_RSLT_IN_TEAM, Sound::SoundId::SE_RSLT_OUT);
}

void CtrlRaceResultTeam::refresh(u32 playerId, u32 characterId, u32 teamId, u32 positionId) {
    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    setMessage("mii_name", Registry::GetCharacterMessageId(characterId, true));
    setPicture("chara_icon", Registry::GetCharacterPane(characterId));
    setPicture("chara_icon_sha", Registry::GetCharacterPane(characterId));
    m_animator.setAnimation(GroupId::Loop, 0, 0.0f);
    m_animator.setAnimation(GroupId::Select, playerId >= localPlayerCount, 0.0f);
    m_animator.setAnimation(GroupId::Select2, playerId >= localPlayerCount, 0.0f);
    m_animator.setAnimationInactive(GroupId::Position, 0, positionId);
    const char *paneNames[2] = { "black_parts_c_r", "black_parts_c_l" };
    for (size_t i = 0; i < std::size(paneNames); i++) {
        auto *pane = m_mainLayout.findPaneByName(paneNames[i]);
        assert(pane);
        auto *material = pane->getMaterial();
        assert(material);
        auto color = TeamColors::Get(teamId);
        material->tevColors[0] = { color.r, color.g, color.b, color.a };
        material->tevColors[1] = { color.r, color.g, color.b, color.a };
    }
}

void CtrlRaceResultTeam::refresh(u32 score) {
    MessageInfo info{};
    info.intVals[0] = score;
    setMessage("point", 1314, &info);
    setMessage("pts", 1313, &info);
}

} // namespace UI
