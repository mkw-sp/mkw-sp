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
    auto *globalContext = SectionManager::Instance()->globalContext();

    m_playerCount = roomManager->getPlayerCount();
    for (u8 i = 0; i < m_playerCount; i++) {
        auto *mii = &roomManager->player(i).m_mii;
        m_miiGroup.insertFromRaw(i, mii);
        globalContext->m_playerMiis.insertFromRaw(i, mii);
    }

    globalContext->copyPlayerMiis();
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
    auto *client = SP::RoomClient::Instance();
    assert(client != nullptr);
    client->calc(m_handler);
}

void VotingBackPage::onRefocus() {
    if (m_submitted) {
        push(PageId::Roulette, Anim::Next);
        m_submitted = false;
    }
}

s8 VotingBackPage::_80650b40_stub() {
    return 0;
}

void VotingBackPage::setLocalVote(Registry::Course course) {
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
            menuScenario.players[i].type = System::RaceConfig::Player::Type::CPU;
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

void VotingBackPage::Handler::onReceivePulse(s8 playerId) {
    m_page.m_selected[playerId] = true;
}

void VotingBackPage::Handler::onReceiveInfo(s8 playerId, Registry::Course course,
        u32 selectedPlayer, u32 character, u32 vehicle) {
    SP::RoomManager *roomManager = SP::RoomManager::Instance();
    System::RaceConfig *raceConfig = System::RaceConfig::Instance();
    raceConfig->menuScenario().players[playerId].characterId = character;
    raceConfig->menuScenario().players[playerId].vehicleId = vehicle;

    for (u8 i = 0; i < 12; i++) {
        if (roomManager->getPlayerOrder(i) == playerId) {
            m_page.m_courseVotes[i] = course;
        }
    }

    if (playerId + 1 == m_page.m_playerCount) {
        m_page.setPlayerTypes();
        auto *roulettePage = SectionManager::Instance()->currentSection()->page<PageId::Roulette>();
        roulettePage->initSelectingStage(selectedPlayer);
    }
}

void VotingBackPage::Handler::onError(const wchar_t *errorMessage) {
    auto *sectionManager = SectionManager::Instance();
    if (errorMessage == nullptr) {
        sectionManager->transitionToError(30001);
    } else {
        MessageInfo info;
        info.strings[0] = errorMessage;
        sectionManager->transitionToError(30003, info);
    }
}

} // namespace UI
