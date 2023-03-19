#include "OnlineFriendRemovePage.hh"

#include "game/ui/FriendButton.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/YesNoPage.hh"
#include "game/ui/page/OnlineFriendListPage.hh"

namespace UI {

void OnlineFriendRemovePage::onRefocus() {
    auto section = SectionManager::Instance()->currentSection();
    auto yesNoPage = section->page<PageId::YesNoPopup>();

    if (yesNoPage->m_choice == 0) {
        auto friendListPage = section->page<PageId::OnlineFriendList>();
        auto onlinePage = section->page<PageId::OnlineConnectionManager>();

        auto friendIdx = friendListPage->m_selectedFriend;
        auto friendInfo = onlinePage->getFriend(friendIdx);

        onlinePage->requestRegisterFriend(friendInfo->client_id.device,
                friendInfo->client_id.licence, true);
    }

    m_replacement = PageId::OnlineFriendList;
    startReplace(Anim::Prev, 0);
}

void OnlineFriendRemovePage::onYesButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto section = SectionManager::Instance()->currentSection();
    auto friendListPage = section->page<PageId::OnlineFriendList>();
    auto onlinePage = section->page<PageId::OnlineConnectionManager>();

    auto friendIdx = friendListPage->m_selectedFriend;
    auto friendInfo = onlinePage->getFriend(friendIdx);
    if (!friendInfo->full_friend) {
        onlinePage->requestRegisterFriend(friendInfo->client_id.device,
                friendInfo->client_id.licence, true);

        m_replacement = PageId::OnlineFriendList;
        startReplace(Anim::Prev, button->getDelay());
        return;
    }

    auto yesNoPage = section->page<PageId::YesNoPopup>();

    yesNoPage->reset();
    yesNoPage->setWindowMessage(5217);
    yesNoPage->configureButton(0, 2002, nullptr, Anim::None, nullptr);
    yesNoPage->configureButton(1, 2003, nullptr, Anim::None, nullptr);
    yesNoPage->setDefaultChoice(1);

    push(PageId::YesNoPopup, Anim::Next);
    button->setFrontSoundId(Sound::SoundId::SE_DUMMY);
}

} // namespace UI
