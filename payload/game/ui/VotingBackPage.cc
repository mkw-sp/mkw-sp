#include "VotingBackPage.hh"

#include "game/ui/RoulettePage.hh"
#include "game/ui/SectionManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/ResourceManager.hh"

namespace UI {

VotingBackPage::VotingBackPage() : m_handler(*this) {}

VotingBackPage::~VotingBackPage() = default;

void VotingBackPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);

    m_miiGroup.init(12, 4, nullptr);

    auto *roomManager = SP::RoomManager::Instance();
    assert(roomManager);

    m_playerCount = roomManager->getPlayerCount();
    for (u8 i = 0; i < m_playerCount; i++) {
        m_miiGroup.insertFromRaw(i, roomManager->getPlayer(i)->getMii());
    }
}

void VotingBackPage::onActivate() {
    auto *roomManager = SP::RoomManager::Instance();
    for (u8 i = 0; i < m_selected.size(); i++) {
        if (roomManager->getPlayerOrder(i) != -1) {
            m_selected[i] = true;
        }
    }
}

void VotingBackPage::afterCalc() {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    if (sectionId == SectionId::VotingServer) {
        auto *server = SP::RoomServer::Instance();
        if (server && !server->calc(m_handler)) {
            SP::RoomServer::DestroyInstance();
        }
    } else {
        auto *client = SP::RoomClient::Instance();
        if (client && !client->calc(m_handler)) {
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

void VotingBackPage::setPlayerTypes() {
    SP::RoomManager *roomManager = SP::RoomManager::Instance();
    System::RaceConfig::Scenario &menuScenario = System::RaceConfig::Instance()->menuScenario();
    for (u8 i = 0; i < 12; i++) {
        if (roomManager->isPlayerLocal(i)) {
            menuScenario.players[i].type = System::RaceConfig::Player::Type::Local;
            continue;
        }

        if (i < m_playerCount) {
            menuScenario.players[i].type = System::RaceConfig::Player::Type::Online;
            continue;
        }

        menuScenario.players[i].type = System::RaceConfig::Player::Type::None;
    }
}

void VotingBackPage::setSubmitted(bool submitted) {
    m_submitted = submitted;
}

bool VotingBackPage::getSubmitted() {
    return m_submitted;
}

VotingBackPage *VotingBackPage::Instance() {
    auto *section = SectionManager::Instance()->currentSection();
    return section->page<PageId::VotingBack>();
}

VotingBackPage::Handler::Handler(VotingBackPage &page) : m_page(page) {}

VotingBackPage::Handler::~Handler() = default;

void VotingBackPage::Handler::onReceivePulse(u32 playerId) {
    m_page.m_selected[playerId] = true;
}

void VotingBackPage::Handler::onReceiveInfo(u32 playerId, s32 course, u32 selectedPlayer,
        u32 character, u32 vehicle) {
    System::RaceConfig *raceConfig = System::RaceConfig::Instance();
    raceConfig->menuScenario().players[playerId].characterId = character;
    raceConfig->menuScenario().players[playerId].vehicleId = vehicle;

    m_page.m_courseVotes[playerId] = course;

    if (playerId + 1 == m_page.m_playerCount) {
        m_page.setPlayerTypes();
        auto *roulettePage = SectionManager::Instance()->currentSection()->page<PageId::Roulette>();
        roulettePage->initSelectingStage(selectedPlayer);
    }
}

} // namespace UI
