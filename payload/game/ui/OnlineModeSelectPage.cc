#include "OnlineModeSelectPage.hh"

#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"

#include <sp/trackPacks/TrackPackManager.hh>

namespace UI {

void OnlineModeSelectPage::setRatings(u16 vsRating, u16 btRating) {
    MessageInfo info = {};

    info.intVals[0] = vsRating;
    m_raceButton.setMessage("go", 4202, &info);

    info.intVals[0] = btRating;
    m_battleButton.setMessage("go", 4203, &info);
}

// Replacement needed to set VR and BR from the OnlineConnectionManager
void OnlineModeSelectPage::onInit() {
    auto currentSectionId = SectionManager::Instance()->currentSection()->id();
    auto isSplitscreen = (currentSectionId >= SectionId::OnlineMultiConfigure) &&
            (currentSectionId <= SectionId::WifiMultiBtVoting);

    m_inputManager.init(1, isSplitscreen);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    initChildren(5);
    insertChild(0, &m_titleText, 0);
    insertChild(1, &m_raceButton, 0);
    insertChild(2, &m_battleButton, 0);
    insertChild(3, &m_backButton, 0);
    insertChild(4, &m_instructionText, 0);

    m_raceButton.load("button", "WifiMenuModeSelect", "ButtonRankMatch", 1, 0, false);
    m_raceButton.setFrontHandler(&m_onButtonFront, false);
    m_raceButton.setSelectHandler(&m_onButtonSelect, false);
    m_raceButton.selectDefault(0);
    m_raceButton.m_index = 0;

    m_battleButton.load("button", "WifiMenuModeSelect", "ButtonBattle", 1, 0, false);
    m_battleButton.setFrontHandler(&m_onButtonFront, false);
    m_battleButton.setSelectHandler(&m_onButtonSelect, false);
    m_battleButton.m_index = 1;

    m_backButton.load("button", "Back", "ButtonBack", 1, 0, false);
    m_backButton.setFrontHandler(&m_onBackFront, false);
    m_backButton.setSelectHandler(&m_onButtonSelect, false);

    m_instructionText.load();
    m_titleText.load(0);
}

// Replacement needed to hook to up OnlineConnectionManager
void OnlineModeSelectPage::onActivate() {
    auto sectionManager = SectionManager::Instance();
    auto section = sectionManager->currentSection();
    auto connectionManager = section->page<PageId::OnlineConnectionManager>();

    if (sectionManager->globalContext()->isVanillaTracks()) {
        m_titleText.setMessage(4000);
    } else {
        MessageInfo info;
        info.strings[0] = SP::TrackPackManager::Instance().getSelectedPack().getPrettyName();
        m_titleText.setMessage(20048, &info);
    }

    auto vsRating = connectionManager->getVsRating();
    auto btRating = connectionManager->getBtRating();
    if (vsRating.has_value() && btRating.has_value()) {
        setRatings(*vsRating, *btRating);
    }
}

void OnlineModeSelectPage::onBack(u32 /* localPlayerId */) {
    m_replacement = PageId::OnlineTop;
    startReplace(Anim::Prev, 0);
}

void OnlineModeSelectPage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto section = SectionManager::Instance()->currentSection();
    auto connectionManager = section->page<PageId::OnlineConnectionManager>();

    connectionManager->m_gamemode = button->m_index;

    m_replacement = PageId::RandomMatching;
    startReplace(Anim::Next, button->getDelay());
}

void OnlineModeSelectPage::onButtonSelect(PushButton *button, u32 /* localPlayerId */) {
    m_instructionText.setMessage(4314 + button->m_index, nullptr);
}

void OnlineModeSelectPage::onBackButtonFront(PushButton * /* button */, u32 localPlayerId) {
    onBack(localPlayerId);
}

PageId OnlineModeSelectPage::getReplacement() {
    return m_replacement;
}

} // namespace UI
