#include "MenuServerPage.hh"

namespace UI {

MenuServerPage::MenuServerPage() : m_serverHandler(*this), m_clientHandler(*this) {}

MenuServerPage::~MenuServerPage() = default;

MenuServerPage::ServerHandler::ServerHandler(MenuServerPage &page) : m_page(page) {}

MenuServerPage::ServerHandler::~ServerHandler() = default;

MenuServerPage::ClientHandler::ClientHandler(MenuServerPage &page) : m_page(page) {}

MenuServerPage::ClientHandler::~ClientHandler() = default;

} // namespace UI
