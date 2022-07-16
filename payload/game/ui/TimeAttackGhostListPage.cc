#include "TimeAttackGhostListPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

TimeAttackGhostListPage::TimeAttackGhostListPage() = default;

TimeAttackGhostListPage::~TimeAttackGhostListPage() = default;

PageId TimeAttackGhostListPage::getReplacement() {
    return m_replacement;
}

void TimeAttackGhostListPage::onInit() {
    SectionManager *sectionManager = SectionManager::Instance();

    auto &player = System::RaceConfig::Instance()->menuScenario().players[0];
    switch (sectionManager->currentSection()->id()) {
    case SectionId::SingleChangeCourse:
    case SectionId::SingleChangeGhostData: {
        const GlobalContext *context = sectionManager->globalContext();
        player.type = System::RaceConfig::Player::Type::Local;
        player.vehicleId = context->m_timeAttackVehicleId;
        player.characterId = context->m_timeAttackCharacterId;
        break;
    }
    default:
        break;
    }

    m_input.init(0x1, /* isMultiPlayer = */ false);
    setInputManager(&m_input);
    m_input.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(9);
    insertChild(0, &m_titleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_ghostSelects[0], 0);
    insertChild(3, &m_ghostSelects[1], 0);
    insertChild(4, &m_sheetSelect, 0);
    insertChild(5, &m_sheetLabel, 0);
    insertChild(6, &m_messageWindow, 0);
    insertChild(7, &m_okButton, 0);
    insertChild(8, &m_backButton, 0);

    m_titleText.load(/* isOptions = */ false);
    m_settingsButton.load("button", "RuleSetting", "ButtonRuleSetting", 0x1, false, false);
    m_ghostSelects[0].load();
    m_ghostSelects[1].load();
    m_sheetSelect.load("button", "TimeAttackGhostListArrowRight", "ButtonArrowRight",
            "TimeAttackGhostListArrowLeft", "ButtonArrowLeft", 0x1, false, false);
    m_sheetLabel.load(
            "control", "TimeAttackGhostListPageNum", "TimeAttackGhostListPageNum", NULL);
    m_messageWindow.load("message_window", "TimeAttackGhostListMessageWindowHalf",
            "MessageWindowHalf");
    m_okButton.load("button", "TimeAttackGhostList", "OK", 0x1, false, false);
    m_backButton.load(
            "button", "Back", "ButtonBackPopup", 0x1, false, /* pointerOnly = */ true);

    m_input.setHandler(MenuInputManager::InputId::Back, &m_onBack);
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);
    m_settingsButton.setSelectHandler(&m_onSettingsButtonSelect, false);
    m_settingsButton.setDeselectHandler(&m_onSettingsButtonDeselect, false);
    m_sheetSelect.setRightHandler(&m_onSheetSelectRight);
    m_sheetSelect.setLeftHandler(&m_onSheetSelectLeft);
    m_okButton.setSelectHandler(&m_onOKButtonSelect, false);
    m_okButton.setFrontHandler(&m_onOKButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_titleText.setMessage(3407);
    m_settingsButton.setMessageAll(10076);
    m_messageWindow.setMessageAll(3101);
}

void TimeAttackGhostListPage::onActivate() {
    m_okButton.selectDefault(0);
    auto *section = SectionManager::Instance()->currentSection();
    auto *ghostManagerPage = section->page<PageId::GhostManager>();
    m_ghostList = ghostManagerPage->list();

    const u32 buttonsPerSheet = m_ghostSelects[0].buttons.size();
    m_sheetCount = (m_ghostList->count() + buttonsPerSheet - 1) / buttonsPerSheet;
    m_sheetIndex = 0;
    refreshSheetLabel();

    m_chosenCount = 0;
    m_ghostIsChosen.fill(false);

    m_shownGhostSelect = &m_ghostSelects[0];
    m_hiddenGhostSelect = &m_ghostSelects[1];
    m_shownGhostSelect->show();
    m_hiddenGhostSelect->hide();

    m_lastSelected = -1;

    m_sheetSelect.setVisible(m_sheetCount > 1);
    m_sheetSelect.setPlayerFlags(m_sheetCount <= 1 ? 0x0 : 0x1);

    m_isReplay = false;
    refreshOKButton();

    m_messageWindow.setVisible(m_sheetCount == 0);

    m_replacement = PageId::None;
}

void TimeAttackGhostListPage::chooseGhost(u32 buttonIndex) {
    u32 listIndex = m_sheetIndex * m_ghostSelects[0].buttons.size() + buttonIndex;
    bool chosen = m_shownGhostSelect->buttons[buttonIndex].isChosen();

    m_ghostIsChosen[listIndex] = chosen;
    if (chosen) {
        ++m_chosenCount;
    } else {
        --m_chosenCount;
    }

    refreshOKButton();
}

void TimeAttackGhostListPage::onBack([[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::CourseSelect;
    startReplace(Anim::Prev, 0.0f);
}

void TimeAttackGhostListPage::onSettingsButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *settingsPage = section->page<PageId::Settings>();
    settingsPage->m_replacement = PageId::TimeAttackGhostList;
    m_replacement = PageId::Settings;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void TimeAttackGhostListPage::onSettingsButtonSelect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_sheetSelect.setPointerOnly(true);
}

void TimeAttackGhostListPage::onSettingsButtonDeselect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_sheetSelect.setPointerOnly(false);
}

void TimeAttackGhostListPage::onSheetSelectRight([[maybe_unused]] SheetSelectControl *control,
        [[maybe_unused]] u32 localPlayerId) {
    if (!canSwapGhostSelects()) {
        return;
    }

    if (m_sheetIndex == m_sheetCount - 1) {
        m_sheetIndex = 0;
    } else {
        ++m_sheetIndex;
    }
    refreshSheetLabel();

    m_shownGhostSelect->slideOutToLeft();
    m_hiddenGhostSelect->slideInFromRight();

    swapGhostSelects();
}

void TimeAttackGhostListPage::onSheetSelectLeft([[maybe_unused]] SheetSelectControl *control,
        [[maybe_unused]] u32 localPlayerId) {
    if (!canSwapGhostSelects()) {
        return;
    }

    if (m_sheetIndex == 0) {
        m_sheetIndex = m_sheetCount - 1;
    } else {
        --m_sheetIndex;
    }
    refreshSheetLabel();

    m_shownGhostSelect->slideOutToRight();
    m_hiddenGhostSelect->slideInFromLeft();

    swapGhostSelects();
}

void TimeAttackGhostListPage::onOKButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::TimeAttackModeSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void TimeAttackGhostListPage::onOKButtonSelect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_lastSelected = -1;
}

void TimeAttackGhostListPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::CourseSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void TimeAttackGhostListPage::refreshSheetLabel() {
    m_sheetLabel.setVisible(m_sheetCount != 0);
    MessageInfo info{};
    info.intVals[0] = m_sheetIndex + 1;
    info.intVals[1] = m_sheetCount;
    m_sheetLabel.setMessageAll(2009, &info);
}

bool TimeAttackGhostListPage::canSwapGhostSelects() const {
    if (!m_shownGhostSelect->isShown()) {
        return false;
    }
    if (!m_hiddenGhostSelect->isHidden()) {
        return false;
    }

    return true;
}

void TimeAttackGhostListPage::swapGhostSelects() {
    std::swap(m_shownGhostSelect, m_hiddenGhostSelect);
}

void TimeAttackGhostListPage::refreshOKButton() {
    MessageInfo info{};
    info.intVals[0] = m_chosenCount;
    m_okButton.setMessageAll(10162, &info);
}

} // namespace UI
