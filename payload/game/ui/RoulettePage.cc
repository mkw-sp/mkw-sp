#include "RoulettePage.hh"

#include "game/ui/SectionManager.hh"
#include "game/ui/VotingBackPage.hh"

#include <sp/cs/RoomClient.hh>

namespace UI {

const char *animInfo[] = {
        "Loop", "Loop", nullptr, // group 0
        "Fade", "Hide", "Fadein", "Show", "Fadeout", nullptr, // group 1
        "Local", "Local", "Network", nullptr, // group 2
        "Group", "Individual", "Blue", "Red", nullptr, // group 3
        "Roulette", "Unselected", "Selected", "Decide", nullptr, // group 4
        nullptr // end
};

RoulettePage::RoulettePage() = default;

RoulettePage::~RoulettePage() = default;

void RoulettePage::onInit() {
    switch (SectionManager::Instance()->currentSection()->id()) {
    case SectionId::Voting1PVS:
        m_isBattle = true;
        break;
    case SectionId::Voting1PBalloon:
    case SectionId::Voting1PCoin:
        m_isBattle = false;
        break;
    default:
        break;
    }

    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(15);
    insertChild(12, &m_pageTitleText, 0);
    m_pageTitleText.load(false);

    if (m_isBattle) {
        m_pageTitleText.setMessage(4354, nullptr);
    } else {
        m_pageTitleText.setMessage(4355, nullptr);
    }

    insertChild(13, &m_instructionText, 0);
    m_instructionText.load();

    insertChild(14, &m_voteNum, 0);
    m_voteNum.load("control", "VoteNum", "VoteNum", nullptr);

    char buffer[15];
    for (u8 i = 0; i < 12; i++) {
        insertChild(i, &m_voteControl[i], 0);
        snprintf(buffer, sizeof(buffer), "Vote%02d", i);
        m_voteControl[i].load("Control", "Vote", buffer, animInfo);
    }
}

void RoulettePage::beforeInAnim() {
    m_stage = Stage::Waiting;

    m_playerOrder.fill(-1);

    auto *section = SectionManager::Instance()->currentSection();
    if (section->id() == SectionId::VotingServer) {
        return;
    }

    auto *votingBackPage = section->page<PageId::VotingBack>();
    auto player = SectionManager::Instance()->globalContext()->m_selectPlayer[0];
    bool driftType = SectionManager::Instance()->globalContext()->m_driftIsAuto[0];

    SP_LOG("beforeInAnim: Vote sent!");
    std::optional<SP::RoomManager::Player::Properties> properties =
            SP::RoomManager::createPlayerProperties(player.m_characterId, player.m_vehicleId,
                    driftType);
    SP::RoomClient::Instance()->sendVote(votingBackPage->getLocalVote(), properties);
}

void RoulettePage::beforeCalc() {
    if (m_stage == Stage::Waiting) {
        if (frame() % 5 == 0) {
            for (u8 i = 0; i < 12; i++) {
                if (calcPlayer(i)) {
                    break;
                }
            }
        }
        calcElements();
        return;
    }

    if (m_stage == Stage::Selecting) {
        // Try hovering over new player
        m_timeTotal += m_timeDelta;
        f32 delta = m_timeDelta - 0.001f;
        if (delta < 0.03) {
            delta = 0.03;
        }
        m_timeDelta = delta;

        s32 prevHoverIdx = m_hoverPlayerIdx;
        m_hoverPlayerIdx = (s32)m_timeTotal % m_currentPlayerIdx;

        if (prevHoverIdx == m_hoverPlayerIdx) {
            return;
        }

        // Animate hovering over new player
        m_voteControl[prevHoverIdx].dehoverOldPlayer();
        m_voteControl[m_hoverPlayerIdx].hoverNewPlayer();

        // Finalize vote
        if (m_timeDelta > 0.05) {
            return;
        }

        if (m_hoverPlayerIdx != m_selectedPlayer) {
            return;
        }

        m_voteControl[m_hoverPlayerIdx].select(m_selectedTrackMessageId);

        m_delay = 180;
        m_stage = Stage::Selected;
        s32 instructionMessage = m_isBattle ? 4363 : 4362;
        m_instructionText.setMessage(instructionMessage, nullptr);
        return;
    }

    if (m_stage == Stage::Selected) {
        m_delay--;
        if (m_delay != 0xFF) {
            return;
        }

        // TODO: Change section to race
        // Probably belongs in another PR
    }
}

void RoulettePage::initSelectingStage(u32 selectedPlayer) {
    auto *votingBackPage = SectionManager::Instance()->currentSection()->page<PageId::VotingBack>();
    for (u8 i = 0; i < votingBackPage->getPlayerCount(); i++) {
        m_voteControl[i].setMessageAll(votingBackPage->getCourseVote(i) + 9300, nullptr);
    }

    m_timeDelta = 0.3;
    m_timeTotal = 0.0;
    m_hoverPlayerIdx = 0;
    m_selectedPlayer = selectedPlayer;
    m_selectedTrackMessageId = votingBackPage->getCourseVote(selectedPlayer) + 9300;
    m_stage = Stage::Selecting;
}

bool RoulettePage::calcPlayer(u8 playerIdx) {
    auto *votingBackPage = SectionManager::Instance()->currentSection()->page<PageId::VotingBack>();
    if (playerIdx >= votingBackPage->getPlayerCount()) {
        return false;
    }

    if (m_playerOrder[playerIdx] >= 0) {
        return false;
    }

    if (!votingBackPage->hasSelected(playerIdx)) {
        return false;
    }

    m_voteControl[m_currentPlayerIdx].onNewVote(votingBackPage->getMiiGroup(), playerIdx);
    m_playerOrder[playerIdx] = m_currentPlayerIdx;
    m_currentPlayerIdx++;
    return true;
}

void RoulettePage::calcElements() {
    MessageInfo info;
    auto *votingBackPage = SectionManager::Instance()->currentSection()->page<PageId::VotingBack>();
    info.intVals[1] = votingBackPage->getPlayerCount();

    u8 voteCount = 0;
    for (u8 i = 0; i < 12; i++) {
        if (m_playerOrder[i] >= 0) {
            voteCount++;
        }
    }

    info.intVals[0] = voteCount;
    m_voteNum.setMessageAll(2009, &info);
}

RoulettePage::VoteControl::VoteControl() = default;

RoulettePage::VoteControl::~VoteControl() = default;

void RoulettePage::VoteControl::initSelf() {
    m_animator.setAnimation(0, 0, 0.0);
    m_animator.setAnimation(1, 0, 0.0);
    m_animator.setAnimation(2, 1, 0.0);
    m_animator.setAnimation(3, 0, 0.0);
    m_animator.setAnimation(4, 0, 0.0);
    setVisible(false);
}

void RoulettePage::VoteControl::calcSelf() {}

void RoulettePage::VoteControl::onNewVote(MiiGroup *miiGroup, u8 playerIdx) {
    setMiiPicture("chara_icon", miiGroup, playerIdx, 2);
    setMiiPicture("chara_icon_sha", miiGroup, playerIdx, 2);

    m_animator.setAnimation(2, 0, 0.0);
    m_animator.setAnimation(3, 0, 0.0);
    m_animator.setAnimation(1, 1, 0.0);
    setVisible(true);
    playSound(Sound::SoundId::SE_UI_CRS_VOTE, -1);
}

void RoulettePage::VoteControl::dehoverOldPlayer() {
    m_animator.setAnimation(4, 0, 0.0);
}

void RoulettePage::VoteControl::hoverNewPlayer() {
    m_animator.setAnimation(4, 1, 0.0);
    playSound(Sound::SoundId::SE_UI_CRS_ROULETTE, -1);
}

void RoulettePage::VoteControl::select(u32 messageId) {
    setMessageAll(messageId, nullptr);
    m_animator.setAnimation(4, 2, 0.0);
    playSound(Sound::SoundId::SE_UI_CRS_ROULETTE_DECIDE, -1);
}

} // namespace UI