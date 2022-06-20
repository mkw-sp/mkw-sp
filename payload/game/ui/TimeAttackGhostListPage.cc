#include "TimeAttackGhostListPage.hh"

#define this self
extern "C" {
#include "game/system/RaceConfig.h"
#include "game/ui/GhostManagerPage.h"
#include "game/ui/RaceConfirmPage.h"
#include "game/ui/SectionManager.h"
}
#undef this

#include "game/ui/SectionManager.hh"

extern "C" {
#include "TimeAttackGhostListPage.h"
}

namespace UI {

TimeAttackGhostListPage::TimeAttackGhostListPage() = default;
TimeAttackGhostListPage::~TimeAttackGhostListPage() = default;

void TimeAttackGhostListPage::onBack([[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::CourseSelect;
    startReplace(Anim::Prev, 0.0f);
}

void TimeAttackGhostListPage::refreshLaunchButton() {
    if (m_chosenCount == 0) {
        m_switchLabel.setVisible(false);
        m_launchButton.setMessageAll(6016);
    } else if (m_chosenCount == 1) {
        m_switchLabel.setVisible(true);
        const u32 messageId = m_isReplay ? 10001 : 10000;
        m_launchButton.setMessageAll(messageId);
    } else {
        const u32 messageId = m_isReplay ? 10003 : 10002;
        MessageInfo info = {};
        info.intVals[0] = m_chosenCount;
        m_launchButton.setMessageAll(messageId, &info);
    }
}

void TimeAttackGhostListPage::onOption([[maybe_unused]] u32 localPlayerId) {
    if (!m_switchLabel.getVisible()) {
        return;
    }
    m_isReplay ^= true;
    refreshLaunchButton();
    playSfx(m_isReplay ? 0x15 : 0x14, -1);
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

void TimeAttackGhostListPage::refreshSheetLabel() {
    m_sheetLabel.setVisible(m_sheetCount != 0);
    MessageInfo info = {};
    info.intVals[0] = m_sheetIndex + 1;
    info.intVals[1] = m_sheetCount;
    m_sheetLabel.setMessageAll(2009, &info);
}

void TimeAttackGhostListPage::swapGhostSelects() {
    std::swap(m_shownGhostSelect, m_hiddenGhostSelect);
}

void TimeAttackGhostListPage::onSheetSelectRight(
        [[maybe_unused]] SheetSelectControl *control,
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

void TimeAttackGhostListPage::onSheetSelectLeft(
        [[maybe_unused]] SheetSelectControl *control,
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

void TimeAttackGhostListPage::onLaunchButtonFront(
        [[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    push(PageId::RaceConfirm, Anim::Next);
}

void TimeAttackGhostListPage::onLaunchButtonSelect(
        [[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    m_lastSelected = -1;
}

void TimeAttackGhostListPage::onBackButtonFront(
        PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::CourseSelect;
    const f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

PageId TimeAttackGhostListPage::getReplacement() {
    return m_replacement;
}

void TimeAttackGhostListPage::onInit() {
    SectionManager *sectionMgr = SectionManager::Instance();

    RaceConfigPlayer &player = s_raceConfig->menuScenario.players[0];
    switch (sectionMgr->currentSection()->id()) {
    case SectionId::ChangeCourse:
    case SectionId::ChangeGhostData: {
        const GlobalContext *cx = sectionMgr->globalContext();
        player.type = PLAYER_TYPE_LOCAL;
        player.vehicleId = cx->m_timeAttackVehicleId;
        player.characterId = cx->m_timeAttackCharacterId;
        break;
    }
    default:
        break;
    }

    m_input.init(0x1, /* isMultiPlayer = */ false);
    setInputManager(&m_input);
    m_input.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(9);
    insertChild(0, &m_titleText, 0);
    insertChild(1, &m_switchLabel, 0);
    insertChild(2, &m_ghostSelects[0], 0);
    insertChild(3, &m_ghostSelects[1], 0);
    insertChild(4, &m_sheetSelect, 0);
    insertChild(5, &m_sheetLabel, 0);
    insertChild(6, &m_messageWindow, 0);
    insertChild(7, &m_launchButton, 0);
    insertChild(8, &m_backButton, 0);

    m_titleText.load(/* isOptions = */ false);

    const char *groups[] = {
        nullptr,
        nullptr,
    };
    m_switchLabel.load("control", "ClassChange", "ClassChange", groups);

    m_ghostSelects[0].load();
    m_ghostSelects[1].load();
    m_sheetSelect.load("button", "TimeAttackGhostListArrowRight", "ButtonArrowRight",
            "TimeAttackGhostListArrowLeft", "ButtonArrowLeft", 0x1, false, false);
    m_sheetLabel.load(
            "control", "TimeAttackGhostListPageNum", "TimeAttackGhostListPageNum", NULL);
    m_messageWindow.load("message_window", "MessageWindowHalf", "MessageWindowHalf");
    m_launchButton.load("button", "TimeAttackGhostList", "Launch", 0x1, false, false);
    m_backButton.load(
            "button", "Back", "ButtonBackPopup", 0x1, false, /* pointerOnly = */ true);

    m_input.setHandler(MenuInputManager::InputId::Back, &m_onBack);
    m_input.setHandler(MenuInputManager::InputId::Option, &m_onOption);
    m_sheetSelect.setRightHandler(&m_onSheetSelectRight);
    m_sheetSelect.setLeftHandler(&m_onSheetSelectLeft);
    m_launchButton.setSelectHandler(&m_onLaunchButtonSelect, false);
    m_launchButton.setFrontHandler(&m_onLaunchButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_titleText.setMessage(3407);
    u32 flags = RegisteredPadManager_getFlags(&s_sectionManager->registeredPadManager, 0);
    u32 padType = REGISTERED_PAD_FLAGS_GET_TYPE(flags);
    u32 messageId = padType == REGISTERED_PAD_TYPE_GC ? 2306 : 2305;
    MessageInfo info = {};
    info.messageIds[0] = messageId;
    m_switchLabel.setMessageAll(3012, &info);
    m_messageWindow.setMessageAll(3101);
}

void TimeAttackGhostListPage::onActivate() {
    m_launchButton.selectDefault(0);
    ::Section *currentSection = s_sectionManager->currentSection;
    GhostManagerPage *ghostManagerPage =
            (GhostManagerPage *)currentSection->pages[PAGE_ID_GHOST_MANAGER];
    ghostList = &ghostManagerPage->list;

    const u32 buttonsPerSheet = m_ghostSelects[0].buttons.size();
    m_sheetCount = (ghostList->count + buttonsPerSheet - 1) / buttonsPerSheet;
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
    m_switchLabel.setVisible(false);
    refreshLaunchButton();

    m_messageWindow.setVisible(m_sheetCount == 0);

    m_replacement = PageId::None;
}

void TimeAttackGhostListPage::onRefocus() {
    ::Page *raceConfirm = s_sectionManager->currentSection->pages[PAGE_ID_RACE_CONFIRM];
    if (!reinterpret_cast<::RaceConfirmPage *>(raceConfirm)->hasConfirmed) {
        return;
    }

    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    u32 playerCount = m_chosenCount;
    if (playerCount == 0 || !m_isReplay) {
        playerCount++;
    }
    for (u32 i = playerCount; i < std::size(menuScenario->players); i++) {
        menuScenario->players[i].type = PLAYER_TYPE_NONE;
    }

    GlobalContext *cx = SectionManager::Instance()->globalContext();
    cx->m_timeAttackGhostType = GHOST_TYPE_SAVED;
    cx->m_timeAttackCourseId = menuScenario->courseId;
    cx->m_timeAttackLicenseId = -1;

    cx->m_timeAttackGhostCount = 0;
    for (u32 i = 0; i < m_ghostIsChosen.size(); i++) {
        if (m_ghostIsChosen[i]) {
            u32 ghostIndex = this->ghostList->indices[i];
            cx->m_timeAttackGhostIndices[cx->m_timeAttackGhostCount++] = ghostIndex;
        }
    }
    ::Section *currentSection = s_sectionManager->currentSection;
    GhostManagerPage *ghostManagerPage =
            (GhostManagerPage *)currentSection->pages[PAGE_ID_GHOST_MANAGER];
    SectionId sectionId;
    if (m_chosenCount == 0) {
        sectionId = SectionId::TimeAttack;
    } else if (m_isReplay) {
        ghostManagerPage->nextRequest = GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_REPLAY;
        sectionId = SectionId::GhostReplay;
    } else {
        ghostManagerPage->nextRequest = GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_RACE;
        sectionId = SectionId::TimeAttack;
    }
    changeSection(sectionId, Anim::Next, 0.0f);
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

    refreshLaunchButton();
}

} // namespace UI

extern "C" void *my_TimeAttackGhostListPage_ct(void *self) {
    return new (self) UI::TimeAttackGhostListPage();
}

static_assert(sizeof_TimeAttackGhostListPage == sizeof(UI::TimeAttackGhostListPage));
