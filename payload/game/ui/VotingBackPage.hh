#pragma once

#include "game/ui/Page.hh"

#include <sp/cs/RoomClient.hh>
#include <sp/cs/RoomServer.hh>

namespace UI {

class VotingBackPage : public Page {
public:
    VotingBackPage();
    ~VotingBackPage() override;
    void onInit() override;
    void afterCalc() override;
    void onRefocus() override;

    REPLACE s8 _80650b40_stub();
    bool hasSelected(u8 playerIdx) { return m_selected[playerIdx]; }
    s8 getPlayerCount() { return m_playerCount; }
    s32 getLocalVote() { return m_localVote; }

    void setBattle(bool isBattle) { m_isBattle = isBattle; }
    void setLocalVote(s32 course);

    static VotingBackPage* Instance();

private:
    class ServerHandler : public SP::RoomServer::Handler {
    public:
        ServerHandler(VotingBackPage &page);
        ~ServerHandler();

        void onSelect(u32 playerId) override;

    private:
        VotingBackPage &m_page;
    };

    class ClientHandler : public SP::RoomClient::Handler {
    public:
        ClientHandler(VotingBackPage &page);
        ~ClientHandler();

        void onSelect(u32 playerId) override;

    private:
        VotingBackPage &m_page;
    };

    struct Packet {
        s32 m_courseId;
        s32 m_characterId;
        s32 m_vehicleId;
        bool m_driftIsAuto;
    };

    MenuInputManager m_inputManager;
    ServerHandler m_serverHandler;
    ClientHandler m_clientHandler;
    Packet m_packet[12];
    MiiGroup m_miiGroup;

    s32 m_localVote;
    bool m_selected[12];
    s8 m_playerCount;
    bool m_isBattle;
    bool m_submitted = false;
};

} // namespace UI
