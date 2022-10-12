#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class RoulettePage : public Page {
public:
    RoulettePage();
    ~RoulettePage() override;
    void onInit() override;
    void beforeInAnim() override;
    void beforeCalc() override;

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

        void onNewVote();
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
    s8 m_playerOrder[12];
    s8 m_currentPlayerIdx = 0;
    s8 m_selectedPlayer = 0;
    u8 m_delay;
    bool m_isBattle;
    s32 m_selectedTrackMessageId;
};

} // namespace UI
