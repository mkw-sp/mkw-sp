#include "VotingBackPage.hh"

#include "game/ui/RoulettePage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

VotingBackPage::VotingBackPage() : m_serverHandler(*this), m_clientHandler(*this) {}

VotingBackPage::~VotingBackPage() = default;

void VotingBackPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);

    m_miiGroup.init(12, 4, nullptr);

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (sectionId == SectionId::VotingServer) {
        auto *server = SP::RoomServer::Instance();
        assert(server);

        m_playerCount = server->getPlayerCount();
    } else {
        auto *client = SP::RoomClient::Instance();
        assert(client);

        m_playerCount = client->getPlayerCount();
        for (u8 i = 0; i < m_playerCount; i++) {
            m_miiGroup.insertFromRaw(i, client->getPlayerMii(i));
        }
    }

    // TODO: we need to sync player order here
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

void VotingBackPage::onRefocus() {
    if (m_submitted) {
        push(PageId::Roulette, Anim::Next);
    }
}

s8 VotingBackPage::_80650b40_stub() {
    return 0;
}

void VotingBackPage::setLocalVote(s32 course) {
    m_localVote = course;
}

VotingBackPage *VotingBackPage::Instance() {
    auto *section = SectionManager::Instance()->currentSection();
    return section->page<PageId::VotingBack>();
}

VotingBackPage::ServerHandler::ServerHandler(VotingBackPage &page) : m_page(page) {}

VotingBackPage::ServerHandler::~ServerHandler() = default;

void VotingBackPage::ServerHandler::onSelect(u32 playerId) {
    m_page.m_selected[playerId] = true;
}

VotingBackPage::ClientHandler::ClientHandler(VotingBackPage &page) : m_page(page) {}

VotingBackPage::ClientHandler::~ClientHandler() = default;

void VotingBackPage::ClientHandler::onSelect(u32 playerId) {
    m_page.m_selected[playerId] = true;
}

void VotingBackPage::ClientHandler::onReceiveVote(u32 playerId, s32 course, u32 selectedPlayer) {
    m_page.m_courseVotes[playerId] = course;
    if (playerId + 1 == m_page.m_playerCount) {
        auto *roulettePage = SectionManager::Instance()->currentSection()->page<PageId::Roulette>();
        roulettePage->initSelectingStage(selectedPlayer);
    }
}

} // namespace UI
