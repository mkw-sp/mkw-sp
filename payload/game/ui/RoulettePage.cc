#include "RoulettePage.hh"

#include "game/ui/SectionManager.hh"
#include "game/ui/VotingBackPage.hh"

#include <sp/cs/RoomClient.hh>

namespace UI {

const char *animInfo[] = {"Loop", "Loop", nullptr, "Fade", "Hide", "Fadein", "Show", "Fadeout",
        nullptr, "Local", "Local", "Network", nullptr, "Group", "Individual", "Blue", "Red",
        nullptr, "Roulette", "Unselected", "Selected", "Decide", nullptr};

RoulettePage::RoulettePage() {
    for (u8 i = 0; i < 12; i++) {
        m_playerOrder[i] = -1;
    }
}

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

    for (u8 i = 0; i < 12; i++) {
        m_playerOrder[i] = -1;
    }

    auto *section = SectionManager::Instance()->currentSection();
    if (section->id() == SectionId::VotingServer) {
        return;
    }

    auto *votingBackPage = section->page<PageId::VotingBack>();
    auto player = SectionManager::Instance()->globalContext()->m_selectPlayer[0];
    bool driftType = SectionManager::Instance()->globalContext()->m_driftIsAuto[0];

    SP_LOG("beforeInAnim: Vote sent!");
    SP::RoomClient::Instance()->sendVote(votingBackPage->getLocalVote(), player.m_characterId,
            player.m_vehicleId, driftType);
}

void RoulettePage::beforeCalc() {
    if (m_stage == Stage::Waiting) {
        if (frame() % 5 == 0) {
            // FIXME: We enter this block every frame ... Section::calc() gone wrong?
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
        f32 delta = m_timeDelta - 0.001;
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
        m_voteControl[prevHoverIdx].setAnimation(4, 0, 0.0);
        m_voteControl[m_hoverPlayerIdx].setAnimation(4, 1, 0.0);
        m_voteControl[m_hoverPlayerIdx].playSfx(69, -1);

        // Finalize vote
        if (m_timeDelta > 0.05) {
            return;
        }

        if (m_hoverPlayerIdx != m_selectedPlayer) {
            return;
        }

        m_voteControl[m_hoverPlayerIdx].setMessageAll(m_selectedTrackMessageId, nullptr);
        m_voteControl[m_hoverPlayerIdx].setAnimation(4, 2, 0.0);
        m_voteControl[m_hoverPlayerIdx].playSfx(70, -1);

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

bool RoulettePage::calcPlayer(u8 playerIdx) {
    auto *votingBackPage = SectionManager::Instance()->currentSection()->page<PageId::VotingBack>();
    if (playerIdx >= votingBackPage->getPlayerCount()) {
        return false;
    }

    if (m_playerOrder[playerIdx] >= 0) {
        return false;
    }

    // TODO: add "submitted" check from RoomClient

    m_voteControl[playerIdx].onNewVote();
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
    setAnimation(0, 0, 0.0);
    setAnimation(1, 0, 0.0);
    setAnimation(2, 1, 0.0);
    setAnimation(3, 0, 0.0);
    setAnimation(4, 0, 0.0);
    setVisible(false);
}

void RoulettePage::VoteControl::calcSelf() {}

void RoulettePage::VoteControl::onNewVote() {
    // TODO: set Mii picture
    setAnimation(2, 0, 0.0);
    setAnimation(3, 0, 0.0);
    setAnimation(1, 1, 0.0);
    setVisible(true);
    playSfx(68, -1);
}

} // namespace UI