#include "FriendMatchingPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/FriendRoomBackPage.hh"
#include "game/ui/GlobePage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/OnlineTeamSelectPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/DriftSelectPage.hh"

#include <sp/cs/RoomClient.hh>

namespace UI {

FriendMatchingPage::FriendMatchingPage() : m_handler(*this) {}

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

    m_roomStarted = false;
    m_roomHasError = false;
}

void FriendMatchingPage::onActivate() {
    m_replacement = PageId::None;
}

void FriendMatchingPage::beforeInAnim() {
    m_messageWindow.show(20008);
    skipInAnim();
}

void FriendMatchingPage::beforeOutAnim() {
    skipOutAnim();
}

void FriendMatchingPage::afterCalc() {
    auto *roomManager = SP::RoomManager::Instance();
    if (roomManager && !roomManager->calc(m_handler)) {
        roomManager->destroyInstance();
        auto *section = SectionManager::Instance()->currentSection();
        if (section->isPageFocused(this)) {
            auto *messagePagePopup = section->page<PageId::MessagePopup>();
            messagePagePopup->reset();
            u32 messageId = section->isPageActive(PageId::FriendRoomBack) ? 20016 : 20015;
            messagePagePopup->setWindowMessage(messageId);
            push(PageId::MessagePopup, Anim::Next);
        } else {
            collapse(Anim::None);
            m_roomHasError = true;
        }
    }
}

void FriendMatchingPage::onRefocus() {
    auto *section = SectionManager::Instance()->currentSection();
    if (m_roomHasError) {
        m_roomHasError = false;
        auto *messagePagePopup = section->page<PageId::MessagePopup>();
        messagePagePopup->reset();

        u32 messageId = section->isPageActive(PageId::FriendRoomBack) ? 20016 : 20015;
        messagePagePopup->setWindowMessage(messageId);
        push(PageId::MessagePopup, Anim::Next);
        return;
    }

    if (m_roomStarted) {
        m_roomStarted = false;
        if (SP::RoomManager::Instance()) {
            auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
            if (m_gamemode == 0) {
                menuScenario.gameMode = System::RaceConfig::GameMode::OfflineVS;
            } else {
                menuScenario.gameMode = System::RaceConfig::GameMode::OfflineBT;
            }
            return startClient();
        }
    }

    auto *globePage = section->page<PageId::Globe>();
    globePage->requestSpinFar();

    m_replacement = PageId::OnlineTop;
    startReplace(Anim::Prev, 0.0f);
}

void FriendMatchingPage::collapse(Anim anim) {
    setAnim(anim);
    auto *section = SectionManager::Instance()->currentSection();
    if (section->isPageFocused(this)) {
        auto *globePage = section->page<PageId::Globe>();
        globePage->requestSpinFar();

        m_replacement = PageId::OnlineTop;
        startReplace(Anim::None, 0.0f);
    } else if (section->isPageActive(PageId::FriendRoomBack)) {
        auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
        friendRoomBackPage->pop(anim);
    }
}

void FriendMatchingPage::prepareStartClient() {
    auto *section = SectionManager::Instance()->currentSection();
    auto *globePage = section->page<PageId::Globe>();
    globePage->requestSpinClose();
    collapse(Anim::Next);
    m_roomStarted = true;
}

void FriendMatchingPage::onBack(u32 /* localPlayerId */) {
    auto *roomManager = SP::RoomManager::Instance();
    if (roomManager) {
        roomManager->destroyInstance();
        collapse(Anim::Prev);
    }
}

void FriendMatchingPage::startClient() {
    auto *client = SP::RoomClient::Instance();
    auto setting = client->getSetting<SP::ClientSettings::Setting::RoomTeamSize>();
    if (setting == SP::ClientSettings::TeamSize::FFA) {
        auto *section = SectionManager::Instance()->currentSection();
        auto *driftSelectPage = section->page<PageId::DriftSelect>();
        driftSelectPage->setReplacementSection(static_cast<SectionId>(m_gamemode + 0x60));

        push(PageId::CharacterSelect, Anim::Next);
    } else {
        push(PageId::OnlineTeamSelect, Anim::Next);
    }
}

FriendMatchingPage::Handler::Handler(FriendMatchingPage &page) : m_page(page) {}

FriendMatchingPage::Handler::~Handler() = default;

void FriendMatchingPage::Handler::onSetup() {
    m_page.m_messageWindow.show(20009);
}

void FriendMatchingPage::Handler::onMain() {
    m_page.m_messageWindow.hide();
    m_page.push(PageId::FriendRoomBack, Anim::Next);
}

void FriendMatchingPage::Handler::onTeamSelect() {
    m_page.m_gamemode = SP::RoomManager::Instance()->gamemode();
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onRoomStart(m_page.m_gamemode);
}

void FriendMatchingPage::Handler::onSelect() {
    m_page.m_gamemode = SP::RoomManager::Instance()->gamemode();
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onRoomStart(m_page.m_gamemode);
}

void FriendMatchingPage::Handler::onPlayerJoin(const System::RawMii *mii, u32 location,
        u16 latitude, u16 longitude, u32 /* regionLineColor */) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onPlayerJoin(*mii, location, latitude, longitude);
}

void FriendMatchingPage::Handler::onPlayerLeave(u32 playerId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onPlayerLeave(playerId);
}

void FriendMatchingPage::Handler::onReceiveComment(u32 playerId, u32 messageId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onReceiveComment(playerId, messageId);
}

void FriendMatchingPage::Handler::onSettingsChange(
        const std::array<u32, SP::RoomSettings::count> &settings) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *friendRoomBackPage = section->page<PageId::FriendRoomBack>();
    friendRoomBackPage->onSettingsChange(settings);
}

void FriendMatchingPage::Handler::onReceiveTeamSelect(u32 playerId, u32 teamId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *onlineTeamSelectPage = section->page<PageId::OnlineTeamSelect>();
    onlineTeamSelectPage->onReceiveTeamSelect(playerId, teamId);
}

void FriendMatchingPage::Handler::onError(const wchar_t * /* errorMessage */) {
    m_page.m_roomHasError = true;
    m_page.collapse(Anim::Next);
}

} // namespace UI
