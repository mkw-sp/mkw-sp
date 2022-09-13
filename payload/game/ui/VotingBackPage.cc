#include "VotingBackPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

VotingBackPage::VotingBackPage() : m_serverHandler(*this), m_clientHandler(*this) {}

VotingBackPage::~VotingBackPage() = default;

void VotingBackPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
}

void VotingBackPage::afterCalc() {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    if (sectionId == SectionId::VotingServer) {
        auto *server = SP::RoomServer::Instance();
        if (server && !server->calc(m_serverHandler)) {
            SP::RoomServer::DestroyInstance();
        }
    } else {
        auto *client = SP::RoomClient::Instance();
        if (client && !client->calc(m_clientHandler)) {
            SP::RoomClient::DestroyInstance();
        }
    }
}

VotingBackPage::ServerHandler::ServerHandler(VotingBackPage &page) : m_page(page) {}

VotingBackPage::ServerHandler::~ServerHandler() = default;

VotingBackPage::ClientHandler::ClientHandler(VotingBackPage &page) : m_page(page) {}

VotingBackPage::ClientHandler::~ClientHandler() = default;

} // namespace UI
