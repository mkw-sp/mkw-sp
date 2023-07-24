#include "RandomMatchingPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/FriendRoomBackPage.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"

#include <sp/cs/RoomClient.hh>

namespace UI {

RandomMatchingPage::RandomMatchingPage() : m_handler(*this) {}
RandomMatchingPage::~RandomMatchingPage() = default;

PageId RandomMatchingPage::getReplacement() {
    return PageId::FriendMatching;
}

void RandomMatchingPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(1);

    insertChild(0, &m_title, 0);

    m_title.load(0);
}

void RandomMatchingPage::onActivate() {
    auto section = SectionManager::Instance()->currentSection();
    auto onlineManager = section->page<PageId::OnlineConnectionManager>();

    onlineManager->startSearch();

    push(PageId::CharacterSelect, Anim::None);
}

void RandomMatchingPage::onRefocus() {
    auto section = SectionManager::Instance()->currentSection();
    auto onlineManager = section->page<PageId::OnlineConnectionManager>();

    if (onlineManager->isCustomTrackpack()) {
        m_title.setMessage(4001);
    } else {
        m_title.setMessage(4000);
    }
}

void RandomMatchingPage::afterCalc() {
    auto sectionManager = SectionManager::Instance();
    auto section = sectionManager->currentSection();
    if (!section->isPageFocused(this)) {
        return;
    }

    // Check if we found a match, if so, drive the RoomClient.
    auto roomClient = SP::RoomClient::Instance();
    if (roomClient) {
        assert(roomClient->calc(m_handler));
        return;
    }

    auto onlineManager = section->page<PageId::OnlineConnectionManager>();
    auto foundMatchOpt = onlineManager->takeMatchResponse();

    if (foundMatchOpt.has_value()) {
        SP_LOG("Found match!");
        auto foundMatch = *foundMatchOpt;

        auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
        menuScenario.gameMode =
                static_cast<System::RaceConfig::GameMode>(onlineManager->m_gamemode);

        auto port = 21330;
        auto ip = foundMatch.room_ip;
        auto loginInfo = foundMatch.login_info;

        SP::RoomClient::CreateInstance(1, ip, port, loginInfo);
    }
}

RandomMatchingPage::Handler::Handler(RandomMatchingPage &page) : m_page(page) {}
RandomMatchingPage::Handler::~Handler() = default;

void RandomMatchingPage::Handler::onSelect() {
    m_page.changeSection(SectionId::Voting1PVS, Anim::Next, 0);
}

void RandomMatchingPage::Handler::onError(const wchar_t *errorMessage) {
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
