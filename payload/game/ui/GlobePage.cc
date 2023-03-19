#include "GlobePage.hh"

#include "game/scene/globe/GlobeManager.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

void GlobePage::afterCalc() {
    if (state() == Page::State::State4) {
        if (m_state == 4 || m_state == 6) {
            return handleSpinFriend();
        }
    }

    REPLACED(afterCalc)();
}

void GlobePage::requestSpinFriend(const System::Mii *mii, u32 friendIndex, void *callback) {
    m_state = 4;
    m_mii = *mii;
    m_callback = callback;
    m_friendIndex = friendIndex;

    m_nameWindow.clearMessageAll();
    m_nameWindow.setPaneVisible("flag_null", false);

    auto group = m_commentWindow.m_animator.getGroup(0);
    auto currentAnimation = group->getAnimation();

    if (currentAnimation == 1) {
        auto frameSize = group->getDuration(1);
        group->setAnimation(3, 1.0 - group->getFrame() / frameSize);
    } else if (currentAnimation == 2) {
        group->setAnimation(3, 0.0);
    }

    auto globeManager = Scene::GlobeManager::Instance();
    globeManager->setLocation(2, 0.0, 0.0);

    auto section = SectionManager::Instance()->currentSection();
    auto onlinePage = section->page<PageId::OnlineConnectionManager>();
    auto friendData = onlinePage->getFriend(friendIndex);

    m_location = friendData->location;
}

void GlobePage::handleSpinFriend() {
    auto commentGroup = m_commentWindow.m_animator.getGroup(0);
    if (commentGroup->getAnimation() != 0) {
        return; // Currently still animating
    }

    auto globeManager = Scene::GlobeManager::Instance();
    globeManager->thunk_34_14();

    showFriendInfo();

    // TODO: Handle calling callback
    m_state = 5;
    playSound(Sound::SoundId::SE_UI_CHAT_ON, -1);
}

void GlobePage::showFriendInfo() {
    auto section = SectionManager::Instance()->currentSection();
    auto onlinePage = section->page<PageId::OnlineConnectionManager>();

    auto friendId = onlinePage->getFriend(m_friendIndex)->client_id;
    auto friendCode = onlinePage->getFriendCode(friendId.device, friendId.licence);

    MessageInfo miiInfo = {};
    MessageInfo codeInfo = {};

    miiInfo.miis[0] = &m_mii;
    codeInfo.intVals[0] = friendCode / 100000000;
    codeInfo.intVals[1] = (friendCode / 10000) % 10000;
    codeInfo.intVals[2] = friendCode % 10000;

    m_nameWindow.setMessage("mii_name", 9501, &miiInfo);
    m_nameWindow.setMessage("shadow", 9501, &miiInfo);

    m_nameWindow.setMessage("user_id", 2110, &codeInfo);
    m_nameWindow.setMessage("user_id_shadow", 2110, &codeInfo);

    // TODO: Handle showing flag correctly
    // m_nameWindow.setPicture("flag", "136");
    // m_nameWindow.setPicture("flag_shadow", "136");
    // m_nameWindow.setPaneVisible("flag_null", true);
}

} // namespace UI
