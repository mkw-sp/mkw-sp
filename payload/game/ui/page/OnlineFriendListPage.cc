#include "OnlineFriendListPage.hh"

#include "game/system/GameScene.hh"
#include "game/ui/GlobePage.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"

#include <cstdio>

namespace UI {

static const char *animInfo[] = {"State", "Offline", "Online", "RandomMatching", "FriendParent",
        nullptr, nullptr};

PageId OnlineFriendListPage::getReplacement() {
    return m_replacement;
}

void OnlineFriendListPage::refresh() {
    auto section = SectionManager::Instance()->currentSection();
    auto onlinePage = section->page<PageId::OnlineConnectionManager>();

    size_t friendCount = onlinePage->getFriendCount();
    if (friendCount > 5) {
        m_friendList.setVisible(true);
        m_friendList.setPlayerFlags(1);
    } else {
        m_friendList.setVisible(false);
        m_friendList.setPlayerFlags(0);
    }

    for (u8 i = 0; i < 5; i += 1) {
        FriendListButton &friendButton = m_friendButtons[i];

        if (i < friendCount) {
            auto friendData = onlinePage->getFriend(i);

            if (friendData->full_friend) {
                SP_LOG("Inserting into mii group: %u", i);

                auto rawMiiData = friendData->mii.bytes;
                m_miiGroup.insertFromRaw(i, reinterpret_cast<System::RawMii *>(&rawMiiData));

                friendButton.refresh(FriendListButton::State::FullFriends, i);
            } else {
                friendButton.refresh(FriendListButton::State::OneSided, i);
            }
        } else {
            friendButton.refresh(FriendListButton::State::Hidden, i);
        }
    }
}

void OnlineFriendListPage::onInit() {
    char variant[16];

    m_inputManager.init(1, 0);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack);

    initChildren(9);
    for (u8 i = 0; i < 5; i += 1) {
        FriendListButton &friendButton = m_friendButtons[i];
        insertChild(i, &friendButton, 0);

        snprintf(variant, sizeof(variant), "ButtonMii%1u", i);
        friendButton.load(animInfo, "button", "FriendListMii", variant, 1, 0);
        friendButton.setSelectHandler(&m_onFriendSelect, false);
        friendButton.setFrontHandler(&m_onFriendFront, false);
        friendButton.m_miiGroup = &m_miiGroup;
    }

    insertChild(5, &m_titleText, 0);
    insertChild(6, &m_friendList, 0);
    insertChild(7, &m_backButton, 0);
    insertChild(8, &m_pageNum, 0);

    m_friendList.load("button", "FriendListArrowRight", "ButtonArrowRight", "FriendListArrowLeft",
            "ButtonArrowLeft", 1, 0, 0);
    m_friendList.setLeftHandler(&m_onFriendListLeft);
    m_friendList.setRightHandler(&m_onFriendListRight);

    m_backButton.load("button", "Back", "ButtonBack", 1, 0, true);
    m_backButton.setFrontHandler(&m_onBackFront, false);

    m_pageNum.load("control", "FriendListPageNum", "FriendListPageNum", 0);

    auto *gameScene = System::GameScene::Instance();
    m_miiGroup.init(30, 7, gameScene->volatileHeapCollection.mem1);

    m_titleText.load(0);
    m_titleText.setMessage(5011);
    m_friendButtons[0].selectDefault(0);
}

void OnlineFriendListPage::onActivate() {
    refresh();

    // HACK: Makes sure the globe spins to the selected
    // friend when the page is activated.
    for (u8 i = 0; i < 5; i += 1) {
        auto &button = m_friendButtons[i];
        if (button.isSelected()) {
            onFriendSelect(&button, 0);
        }
    }
}

void OnlineFriendListPage::onFriendListLeft(SheetSelectControl * /* sheet */,
        u32 /* localPlayerId */) {
    SP_LOG("OnlineFriendListPage::onFriendListLeft");
}

void OnlineFriendListPage::onFriendListRight(SheetSelectControl * /* sheet */,
        u32 /* localPlayerId */) {
    SP_LOG("OnlineFriendListPage::onFriendListRight");
}

void OnlineFriendListPage::onFriendFront(PushButton *button, u32 /* localPlayerId */) {
    auto friendButton = reinterpret_cast<FriendListButton *>(button);
    m_selectedFriend = friendButton->m_friendIndex;

    m_replacement = PageId::WifiFriendFocused;
    startReplace(Anim::Next, button->getDelay());
}

void OnlineFriendListPage::onFriendSelect(PushButton *button, u32 /* localPlayerId */) {
    auto section = SectionManager::Instance()->currentSection();
    auto globePage = section->page<PageId::Globe>();

    auto friendButton = reinterpret_cast<FriendListButton *>(button);

    if (friendButton->m_state == FriendListButton::State::FullFriends) {
        auto mii = m_miiGroup.get(friendButton->m_friendIndex);
        globePage->requestSpinFriend(mii, friendButton->m_friendIndex, nullptr);
    } else if (friendButton->m_state == FriendListButton::State::OneSided) {
        globePage->requestSpinMid();
        globePage->showFriendInfo();
    }
}

void OnlineFriendListPage::onBackFront(PushButton *button, u32 /* localPlayerId */) {
    m_replacement = PageId::OnlineFriendMenu;
    startReplace(Anim::Prev, button ? button->getDelay() : 0);
}

void OnlineFriendListPage::onBack(u32 localPlayerId) {
    onBackFront(nullptr, localPlayerId);
}

} // namespace UI
