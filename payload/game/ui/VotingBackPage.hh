#pragma once

#include "game/ui/Page.hh"

#include <sp/cs/RoomClient.hh>

#include <bitset>

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

    Registry::Course getLocalVote() {
        return m_localVote;
    }

    Registry::Course getCourseVote(u8 playerIdx) {
        return m_courseVotes[playerIdx];
    }

    MiiGroup *getMiiGroup() {
        return &m_miiGroup;
    }

    bool getSubmitted();

    void setBattle(bool isBattle) {
        m_isBattle = isBattle;
    }

    void setLocalVote(Registry::Course course);
    void setPlayerTypes();
    void setSubmitted(bool submitted);

    static VotingBackPage *Instance();

private:
    class Handler : public SP::RoomManager::Handler {
    public:
        Handler(VotingBackPage &page);
        ~Handler();

        void onReceivePulse(s8 playerId) override;
        void onReceiveInfo(s8 playerId, Registry::Course course, u32 selectedPlayer, u32 character,
                u32 vehicle) override;
        void onError(const wchar_t *errorMessage) override;

    private:
        VotingBackPage &m_page;
    };

    MenuInputManager m_inputManager;
    Handler m_handler;
    MiiGroup m_miiGroup;

    Registry::Course m_localVote;
    std::bitset<12> m_selected;
    Registry::Course m_courseVotes[12];
    u8 m_playerCount;
    bool m_isBattle;
    bool m_submitted = false;
};

} // namespace UI
