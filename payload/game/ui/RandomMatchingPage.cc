#include "RandomMatchingPage.hh"

#include "payload/game/ui/SectionManager.hh"

#include <payload/sp/cs/RoomClient.hh>

namespace UI {

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
        m_title.setMessage(0xfa1);
    } else {
        m_title.setMessage(0xfa0);
    }
}

void RandomMatchingPage::afterCalc() {
    auto section = SectionManager::Instance()->currentSection();
    if (!section->isPageFocused(this)) {
        return;
    }

    auto onlineManager = section->page<PageId::OnlineConnectionManager>();
    auto foundMatchOpt = onlineManager->takeMatchResponse();

    if (foundMatchOpt.has_value()) {
        SP_LOG("RandomMatchingPage: Found match!");
        auto foundMatch = *foundMatchOpt;

        auto port = 21330;
        auto ip = foundMatch.room_ip;
        auto loginInfo = foundMatch.login_info;

        SP::RoomClient::CreateInstance(1, ip, port, 0000, loginInfo);
        startReplace(Anim::Next, 0);
    }
}

}
