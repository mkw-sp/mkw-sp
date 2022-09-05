#include "FriendMatchingPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/FriendRoomBackPage.hh"
#include "game/ui/GlobePage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

FriendMatchingPage::FriendMatchingPage() : m_serverHandler(*this), m_clientHandler(*this) {}

FriendMatchingPage::~FriendMatchingPage() = default;

PageId FriendMatchingPage::getReplacement() {
    return m_replacement;
}

void FriendMatchingPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(2);
    insertChild(0, &m_messageWindow, 0);
    insertChild(1, &m_dummyBack, 0);

    m_messageWindow.load("FriendMatchingMessageWindow");
    m_dummyBack.load("button", "Back", "ButtonBack", 0x1);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false);
}

void FriendMatchingPage::onActivate() {
    m_replacement = PageId::None;
}

void FriendMatchingPage::beforeInAnim() {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (sectionId == SectionId::OnlineServer) {
        SP::RoomServer::CreateInstance();
        m_messageWindow.show(20009);
    } else {
        SP::RoomClient::CreateInstance(sectionId == SectionId::OnlineSingle ? 1 : 2);
        m_messageWindow.show(20008);
    }
    skipInAnim();
}

void FriendMatchingPage::beforeOutAnim() {
    skipOutAnim();
}

void FriendMatchingPage::afterCalc() {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    if (sectionId == SectionId::OnlineServer) {
        auto *server = SP::RoomServer::Instance();
        if (server && !server->calc(m_serverHandler)) {
            SP::RoomServer::DestroyInstance();
            collapse();
            auto *messagePagePopup = section->page<PageId::MessagePopup>();
            messagePagePopup->reset();
            u32 messageId = section->isPageActive(PageId::FriendRoomBack) ? 20014 : 20013;
            messagePagePopup->setWindowMessage(messageId);
            push(PageId::MessagePopup, Anim::Next);
        }
    } else {
        auto *client = SP::RoomClient::Instance();
        if (client && !client->calc(m_clientHandler)) {
            SP::RoomClient::DestroyInstance();
            collapse();
            auto *messagePagePopup = section->page<PageId::MessagePopup>();
            messagePagePopup->reset();
            u32 messageId = section->isPageActive(PageId::FriendRoomBack) ? 20016 : 20015;
            messagePagePopup->setWindowMessage(messageId);
            push(PageId::MessagePopup, Anim::Next);
        }
    }
}

void FriendMatchingPage::onRefocus() {
    auto *section = SectionManager::Instance()->currentSection();
    auto *globePage = section->page<PageId::Globe>();
    globePage->requestSpinFar();
    auto sectionId = section->id();
    if (sectionId == SectionId::OnlineServer) {
        changeSection(SectionId::ServicePack, Anim::Prev, 0.0f);
    } else {
        m_replacement = PageId::OnlineTop;
        startReplace(Anim::Prev, 0.0f);
    }
}

void FriendMatchingPage::start() {
    collapse();
    push(PageId::CharacterSelect, Anim::Next);
    System::RaceConfig::Instance()->menuScenario().gameMode = m_gamemode == 0 ? System::RaceConfig::GameMode::OnlinePrivateVS : System::RaceConfig::GameMode::OnlinePrivateBT;
}

void FriendMatchingPage::onBack([[maybe_unused]] u32 localPlayerId) {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    auto *yesNoPagePopup = section->page<PageId::YesNoPopup>();
    u32 windowMessageId = sectionId == SectionId::OnlineServer ? 20017 : 20018;
    yesNoPagePopup->setWindowMessage(windowMessageId);
    yesNoPagePopup->configureButton(0, 2002, nullptr, Anim::Prev, &m_onCloseConfirm);
    yesNoPagePopup->configureButton(1, 2003, nullptr, Anim::Prev, nullptr);
    push(PageId::YesNoPopup, Anim::Next);
}

void FriendMatchingPage::onCloseConfirm([[maybe_unused]] s32 choice,
        [[maybe_unused]] PushButton *button) {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    if (sectionId == SectionId::OnlineServer) {
        auto *server = SP::RoomServer::Instance();
        if (server) {
            SP::RoomServer::DestroyInstance();
            collapse();
        }
    } else {
        auto *client = SP::RoomClient::Instance();
        if (client) {
            SP::RoomClient::DestroyInstance();
            collapse();
        }
    }
}

void FriendMatchingPage::collapse() {
    auto *section = SectionManager::Instance()->currentSection();
    if (section->isPageActive(PageId::FriendRoomBack)) {
        auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
        friendRoomBackPage->pop();
    }
}

FriendMatchingPage::ServerHandler::ServerHandler(FriendMatchingPage &page) : m_page(page) {}

FriendMatchingPage::ServerHandler::~ServerHandler() = default;

void FriendMatchingPage::ServerHandler::onMain() {
    m_page.m_messageWindow.hide();
    m_page.push(PageId::FriendRoomBack, Anim::Next);
}

void FriendMatchingPage::ServerHandler::onPlayerJoin(const System::RawMii *mii, u32 location,
        u16 latitude, u16 longitude) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onPlayerJoin(*mii, location, latitude, longitude);
}

void FriendMatchingPage::ServerHandler::onPlayerLeave(u32 playerId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onPlayerLeave(playerId);
}

void FriendMatchingPage::ServerHandler::onReceiveComment(u32 playerId, u32 messageId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onReceiveComment(playerId, messageId);
}

void FriendMatchingPage::ServerHandler::onSettingsChange(
        const std::array<u32, SP::RoomSettings::count> &settings) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onSettingsChange(settings);
}

void FriendMatchingPage::ServerHandler::onRoomClose(u32 gamemode) {
    m_page.m_gamemode = gamemode;
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onRoomClose(gamemode);
}

FriendMatchingPage::ClientHandler::ClientHandler(FriendMatchingPage &page) : m_page(page) {}

FriendMatchingPage::ClientHandler::~ClientHandler() = default;

void FriendMatchingPage::ClientHandler::onSetup() {
    m_page.m_messageWindow.show(20009);
}

void FriendMatchingPage::ClientHandler::onMain() {
    m_page.m_messageWindow.hide();
    m_page.push(PageId::FriendRoomBack, Anim::Next);
}

void FriendMatchingPage::ClientHandler::onPlayerJoin(const System::RawMii *mii, u32 location,
        u16 latitude, u16 longitude) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onPlayerJoin(*mii, location, latitude, longitude);
}

void FriendMatchingPage::ClientHandler::onPlayerLeave(u32 playerId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onPlayerLeave(playerId);
}

void FriendMatchingPage::ClientHandler::onReceiveComment(u32 playerId, u32 messageId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onReceiveComment(playerId, messageId);
}

void FriendMatchingPage::ClientHandler::onSettingsChange(
        const std::array<u32, SP::RoomSettings::count> &settings) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onSettingsChange(settings);
}

void FriendMatchingPage::ClientHandler::onRoomClose(u32 gamemode) {
    m_page.m_gamemode = gamemode;
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onRoomClose(gamemode);
}

} // namespace UI
