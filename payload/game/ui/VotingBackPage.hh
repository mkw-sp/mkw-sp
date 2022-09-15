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

    REPLACE s8 _80650b40_stub();

private:
    class ServerHandler : public SP::RoomServer::Handler {
    public:
        ServerHandler(VotingBackPage &page);
        ~ServerHandler();

    private:
        VotingBackPage &m_page;
    };

    class ClientHandler : public SP::RoomClient::Handler {
    public:
        ClientHandler(VotingBackPage &page);
        ~ClientHandler();

    private:
        VotingBackPage &m_page;
    };

    MenuInputManager m_inputManager;
    ServerHandler m_serverHandler;
    ClientHandler m_clientHandler;
};

} // namespace UI
