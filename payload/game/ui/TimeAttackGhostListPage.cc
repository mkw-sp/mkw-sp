#include "TimeAttackGhostListPage.hh"

extern "C" {
#include "game/system/GhostFile.h"
}
#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/RaceConfirmPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

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
        player.vehicleId = static_cast<u32>(context->m_localVehicleIds[0]);
        player.characterId = static_cast<u32>(context->m_localCharacterIds[0]);
        break;
    }
    default:
        break;
    }

    m_input.init(0x1, /* isMultiPlayer = */ false);
    setInputManager(&m_input);
    m_input.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(11);
    insertChild(0, &m_titleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_ghostSelects[0], 0);
    insertChild(3, &m_ghostSelects[1], 0);
    insertChild(4, &m_sheetSelect, 0);
    insertChild(5, &m_sheetLabel, 0);
    insertChild(6, &m_messageWindow, 0);
    insertChild(7, &m_aloneButton, 0);
    insertChild(8, &m_raceButton, 0);
    insertChild(9, &m_watchButton, 0);
    insertChild(10, &m_backButton, 0);

    m_titleText.load(/* isOptions = */ false);
    m_settingsButton.load("button", "SettingsButton", "Option", 0x1, false, false);
    m_ghostSelects[0].load();
    m_ghostSelects[1].load();
    m_sheetSelect.load("button", "TimeAttackGhostListArrowRight", "ButtonArrowRight",
            "TimeAttackGhostListArrowLeft", "ButtonArrowLeft", 0x1, false, false);
    m_sheetLabel.load("control", "TimeAttackGhostListPageNum", "TimeAttackGhostListPageNum", NULL);
    m_messageWindow.load("message_window", "TimeAttackGhostListMessageWindowHalf",
            "MessageWindowHalf");
    m_aloneButton.load("button", "TimeAttackGhostList", "Alone", 0x1, false, false);
    m_raceButton.load("button", "TimeAttackGhostListHalf", "Race", 0x1, false, false);
    m_watchButton.load("button", "TimeAttackGhostListHalf", "Watch", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, /* pointerOnly = */ true);

    m_input.setHandler(MenuInputManager::InputId::Back, &m_onBack);
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);
    m_settingsButton.setSelectHandler(&m_onSettingsButtonSelect, false);
    m_settingsButton.setDeselectHandler(&m_onSettingsButtonDeselect, false);
    m_sheetSelect.setRightHandler(&m_onSheetSelectRight);
    m_sheetSelect.setLeftHandler(&m_onSheetSelectLeft);
    m_aloneButton.setFrontHandler(&m_onAloneButtonFront, false);
    m_aloneButton.setSelectHandler(&m_onAloneButtonSelect, false);
    m_raceButton.setFrontHandler(&m_onRaceButtonFront, false);
    m_raceButton.setSelectHandler(&m_onRaceButtonSelect, false);
    m_watchButton.setFrontHandler(&m_onWatchButtonFront, false);
    m_watchButton.setSelectHandler(&m_onWatchButtonSelect, false);
    m_watchButton.setDeselectHandler(&m_onWatchButtonDeselect, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_titleText.setMessage(3407);
    m_messageWindow.setMessageAll(3101);
}

void TimeAttackGhostListPage::onActivate() {
    auto *section = SectionManager::Instance()->currentSection();
    auto *ghostManagerPage = section->page<PageId::GhostManager>();
    m_ghostList = ghostManagerPage->list();
    if (m_cc) {
        ghostManagerPage->repopulate();
    }

    auto *saveManager = System::SaveManager::Instance();
    auto cc = saveManager->getSetting<SP::ClientSettings::Setting::TAClass>();
    if (!m_cc || *m_cc != cc) {
        m_aloneButton.selectDefault(0);

        const u32 buttonsPerSheet = m_ghostSelects[0].buttons.size();
        m_sheetCount = (m_ghostList->count() + buttonsPerSheet - 1) / buttonsPerSheet;
        m_sheetIndex = 0;
        refreshSheetLabel();

        m_chosenCount = 0;
        m_ghostIsChosen.fill(false);

        m_lastSelected = -1;

        m_sheetSelect.setVisible(m_sheetCount > 1);
        m_sheetSelect.setPlayerFlags(m_sheetCount <= 1 ? 0x0 : 0x1);

        refreshLaunchButtons();

        m_messageWindow.setVisible(m_sheetCount == 0);
    }
    m_cc = cc;

    m_shownGhostSelect = &m_ghostSelects[0];
    m_hiddenGhostSelect = &m_ghostSelects[1];
    m_shownGhostSelect->show();
    m_hiddenGhostSelect->hide();

    m_replacement = PageId::None;
}

void TimeAttackGhostListPage::onRefocus() {
    auto *section = SectionManager::Instance()->currentSection();
    auto *raceConfirmPage = section->page<PageId::RaceConfirm>();
    if (!raceConfirmPage->hasConfirmed()) {
        return;
    }

    System::RaceConfig::Instance()->applyEngineClass();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    u32 playerCount = m_chosenCount;
    if (playerCount == 0 || !m_isReplay) {
        playerCount++;
    }
    for (u32 i = playerCount; i < std::size(menuScenario.players); i++) {
        menuScenario.players[i].type = System::RaceConfig::Player::Type::None;
    }

    GlobalContext *context = SectionManager::Instance()->globalContext();
    context->m_timeAttackGhostType = GHOST_TYPE_PERSONAL_BEST;
    context->m_timeAttackCourseId = menuScenario.courseId;
    context->m_timeAttackLicenseId = -1;

    context->m_timeAttackGhostCount = 0;
    for (u32 i = 0; i < m_ghostList->count(); i++) {
        u32 ghostIndex = m_ghostList->indices()[i];
        if (m_ghostIsChosen[ghostIndex]) {
            context->m_timeAttackGhostIndices[context->m_timeAttackGhostCount++] = ghostIndex;
        }
    }

    auto *ghostManagerPage = section->page<PageId::GhostManager>();
    SectionId sectionId;
    if (m_chosenCount == 0) {
        sectionId = SectionId::TA;
    } else if (m_isReplay) {
        ghostManagerPage->requestGhostReplay();
        sectionId = SectionId::GhostReplay;
    } else {
        ghostManagerPage->requestGhostRace(false, false);
        sectionId = SectionId::TA;
    }
    changeSection(sectionId, Anim::Next, 0.0f);
}

void TimeAttackGhostListPage::chooseGhost(u32 buttonIndex) {
    u32 listIndex = m_sheetIndex * m_ghostSelects[0].buttons.size() + buttonIndex;
    u32 ghostIndex = m_ghostList->indices()[listIndex];
    bool chosen = m_shownGhostSelect->buttons[buttonIndex].isChosen();

    m_ghostIsChosen[ghostIndex] = chosen;
    if (chosen) {
        ++m_chosenCount;
    } else {
        --m_chosenCount;
    }

    refreshLaunchButtons();
}

void TimeAttackGhostListPage::onBack(u32 localPlayerId) {
    onBackButtonFront(nullptr, localPlayerId);
}

void TimeAttackGhostListPage::onSettingsButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuSettingsPage = section->page<PageId::MenuSettings>();
    menuSettingsPage->configure(nullptr, PageId::TimeAttackGhostList);
    m_replacement = PageId::MenuSettings;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void TimeAttackGhostListPage::onSettingsButtonSelect(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_sheetSelect.setPointerOnly(true);
    m_watchButton.setPointerOnly(true);

    m_lastSelected = -1;
}

void TimeAttackGhostListPage::onSettingsButtonDeselect(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_sheetSelect.setPointerOnly(false);
    m_watchButton.setPointerOnly(false);
}

void TimeAttackGhostListPage::onSheetSelectRight(SheetSelectControl * /* control */,
        u32 /* localPlayerId */) {
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

void TimeAttackGhostListPage::onSheetSelectLeft(SheetSelectControl * /* control */,
        u32 /* localPlayerId */) {
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

void TimeAttackGhostListPage::onAloneButtonFront(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_isReplay = false;

    push(PageId::RaceConfirm, Anim::Next);
}

void TimeAttackGhostListPage::onAloneButtonSelect(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_lastSelected = -1;
}

void TimeAttackGhostListPage::onRaceButtonFront(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_isReplay = false;

    push(PageId::RaceConfirm, Anim::Next);
}

void TimeAttackGhostListPage::onRaceButtonSelect(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_lastSelected = -1;
}

void TimeAttackGhostListPage::onWatchButtonFront(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_isReplay = true;

    push(PageId::RaceConfirm, Anim::Next);
}

void TimeAttackGhostListPage::onWatchButtonSelect(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_settingsButton.setPointerOnly(true);

    m_lastSelected = -1;
}

void TimeAttackGhostListPage::onWatchButtonDeselect(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_settingsButton.setPointerOnly(false);
}

void TimeAttackGhostListPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    m_lastSelected = -1;
    m_sheetSelect.setPointerOnly(false);
    m_watchButton.setPointerOnly(false);
    m_settingsButton.setPointerOnly(false);
    m_cc.reset();
    SectionManager::Instance()->globalContext()->clearCourses();

    m_replacement = PageId::CourseSelect;
    f32 delay = button ? button->getDelay() : 0;
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

void TimeAttackGhostListPage::refreshLaunchButtons() {
    m_aloneButton.setVisible(m_chosenCount == 0);
    m_aloneButton.setPlayerFlags(m_chosenCount == 0 ? 0x1 : 0x0);
    m_raceButton.setVisible(m_chosenCount != 0);
    m_raceButton.setPlayerFlags(m_chosenCount != 0 ? 0x1 : 0x0);
    m_watchButton.setVisible(m_chosenCount != 0);
    m_watchButton.setPlayerFlags(m_chosenCount != 0 ? 0x1 : 0x0);
    if (m_chosenCount != 0) {
        MessageInfo info{};
        info.intVals[0] = m_chosenCount;
        m_raceButton.setMessageAll(10162, &info);
        m_watchButton.setMessageAll(10163, &info);
    }
}

} // namespace UI
