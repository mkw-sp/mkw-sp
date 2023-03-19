#include "OnlineFriendRegisterPage.hh"

#include "game/ui/AwaitPage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

PageId OnlineFriendRegisterPage::replacedGetReplacement() {
    if (m_replacement == PageId::MenuAwait) {
        return m_replacement;
    } else {
        return PageId::None;
    }
}

void OnlineFriendRegisterPage::onOkButtonFront(PushButton * /* button */, u32 /* localPlayerId */) {
    auto section = SectionManager::Instance()->currentSection();
    auto connectionManager = section->page<PageId::OnlineConnectionManager>();
    auto spinnerPage = section->page<PageId::SPMenuAwait>();

    auto friendCode = m_editBox.getNumber();
    connectionManager->requestRegisterFriend(friendCode >> 0, friendCode >> 32, false);

    SP_LOG("calling reset");
    spinnerPage->reset();
    SP_LOG("calling setWindowMessage");
    spinnerPage->setWindowMessage(20047);

    m_replacement = PageId::MenuAwait;
    startReplace(Anim::None, 0.0);
}

} // namespace UI
