#pragma once

#include "game/ui/Page.hh"

#include <sp/cs/RoomClient.hh>
#include <sp/cs/RoomServer.hh>

namespace UI {

class VotingBackPage : public Page {
public:
    VotingBackPage();
    VotingBackPage(const VotingBackPage &) = delete;
    VotingBackPage(VotingBackPage &&) = delete;
    ~VotingBackPage() override;

    void onInit() override;
    void onActivate() override;
    void afterCalc() override;
    void onRefocus() override;

    REPLACE s8 _80650b40_stub();
    bool hasSelected(u8 playerIdx) {
        return m_selected[playerIdx];
    }

    s8 getPlayerCount() {
        return m_playerCount;
    }

    s32 getLocalVote() {
        return m_localVote;
    }

    s32 getCourseVote(u8 playerIdx) {
        return m_courseVotes[playerIdx];
    }

    MiiGroup *getMiiGroup() {
        return &m_miiGroup;
    }

    bool getSubmitted();

    void setBattle(bool isBattle) {
        m_isBattle = isBattle;
    }

    void setLocalVote(s32 course);
    void setSubmitted(bool submitted);

    static VotingBackPage *Instance();

private:
    class Handler : public SP::RoomManager::Handler {
    public:
        Handler(VotingBackPage &page);
        ~Handler();

        void onReceivePulse(u32 playerId) override;
        void onReceiveInfo(u32 playerId, s32 course, u32 selectedPlayer, u32 character,
                u32 vehicle) override;

    private:
        VotingBackPage &m_page;
    };

    MenuInputManager m_inputManager;
    Handler m_handler;
    MiiGroup m_miiGroup;

    s32 m_localVote;
    std::bitset<12> m_selected;
    s32 m_courseVotes[12];
    u8 m_playerCount;
    bool m_isBattle;
    bool m_submitted = false;
};

} // namespace UI
