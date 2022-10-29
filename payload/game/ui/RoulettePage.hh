#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <sp/cs/RoomManager.hh>

#include <array>
#include <bitset>

namespace UI {

class RoulettePage : public Page {
public:
    RoulettePage();
    ~RoulettePage() override;
    void onInit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void beforeCalc() override;

    void copyPlayerOrder(SP::RoomManager *roomManager);
    void initSelectingStage(u32 selectedPlayer);

private:
    bool calcPlayer(u8 playerIdx);
    void calcElements();

    enum class Stage {
        Waiting,
        Selecting,
        Selected,
    };

    class VoteControl : public LayoutUIControl {
    public:
        VoteControl();
        ~VoteControl() override;
        void initSelf() override;
        void calcSelf() override;

        void onNewVote(MiiGroup *miiGroup, u8 playerIdx);

        void dehoverOldPlayer();
        void hoverNewPlayer();
        void select(u32 messageId);
    };

    MenuInputManager m_inputManager;
    CtrlMenuInstructionText m_instructionText;
    CtrlMenuPageTitleText m_pageTitleText;
    LayoutUIControl m_voteNum;
    VoteControl m_voteControl[12];
    Stage m_stage;
    f32 m_timeTotal;
    f32 m_timeDelta;
    s32 m_hoverPlayerIdx = -1;
    std::array<s8, 12> m_playerOrder;
    std::bitset<12> m_processed;
    s8 m_currentPlayerIdx = 0;
    s8 m_selectedPlayer = 0;
    u8 m_delay;
    bool m_isBattle;
};

} // namespace UI
