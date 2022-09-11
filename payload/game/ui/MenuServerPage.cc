#include "MenuServerPage.hh"

namespace UI {

MenuServerPage::MenuServerPage() : m_serverHandler(*this), m_clientHandler(*this) {}

MenuServerPage::~MenuServerPage() = default;

void MenuServerPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
}

void MenuServerPage::afterCalc() {
    auto *server = SP::RoomServer::Instance();
    if (server && !server->calc(m_serverHandler)) {
        SP::RoomServer::DestroyInstance();
    }
}

MenuServerPage::ServerHandler::ServerHandler(MenuServerPage &page) : m_page(page) {}

MenuServerPage::ServerHandler::~ServerHandler() = default;

MenuServerPage::ClientHandler::ClientHandler(MenuServerPage &page) : m_page(page) {}

MenuServerPage::ClientHandler::~ClientHandler() = default;

} // namespace UI
