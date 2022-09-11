#pragma once

#include "game/ui/Page.hh"

#include <sp/cs/RoomClient.hh>
#include <sp/cs/RoomServer.hh>

namespace UI {

class MenuServerPage : public Page {
public:
    MenuServerPage();
    ~MenuServerPage() override;
    // TODO: public interface functions

private:
    class ServerHandler : public SP::RoomServer::Handler {
    public:
        ServerHandler(MenuServerPage &page);
        ~ServerHandler();

    private:
        MenuServerPage &m_page;
    };

    class ClientHandler : public SP::RoomClient::Handler {
    public:
        ClientHandler(MenuServerPage &page);
        ~ClientHandler();

    private:
        MenuServerPage &m_page;
    };

    ServerHandler m_serverHandler;
    ClientHandler m_clientHandler;
};

} // namespace UI
