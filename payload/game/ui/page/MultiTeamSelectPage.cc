#include "MultiTeamSelectPage.hh"

#include "game/sound/DriverSoundManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/model/MenuModelManager.hh"
#include "game/ui/page/MessagePage.hh"

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

#include <cstdio>

namespace UI {

MultiTeamSelectPage::MultiTeamSelectPage() = default;

MultiTeamSelectPage::~MultiTeamSelectPage() = default;

PageId MultiTeamSelectPage::getReplacement() {
    return m_replacement;
}

void MultiTeamSelectPage::onInit() {
    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;

    m_inputManager.init((1 << localPlayerCount) - 1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(2 + localPlayerCount * 2);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_backButton, 0);
    for (size_t i = 0; i < localPlayerCount; i++) {
        insertChild(2 + i * 2 + 0, &m_driverModels[i], 0);
        insertChild(2 + i * 2 + 1, &m_nullControls[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < localPlayerCount; i++) {
        m_driverModels[i].load("PadEasyModel", i);
        char variant[0x20];
        snprintf(variant, std::size(variant), "%uP", i + 1);
        if (localPlayerCount == 2) {
            m_nullControls[i].load("control", "MultiNULL2S", variant, nullptr);
        } else {
            m_nullControls[i].load("control", "MultiNULL4S", variant, nullptr);
        }
        m_nullControls[i].initChildren(1);
        m_nullControls[i].insertChild(0, &m_teamControls[i]);
        if (localPlayerCount == 2) {
            m_teamControls[i].load(1, 0, "control", "MachineUpDownBase", "MachineUpDownBase",
                    "TeamArrowRight", "ButtonArrowRight", "TeamArrowLeft", "ButtonArrowLeft",
                    m_teamValues[i].animator(), 1 << i, 0, 0, true, true);
            m_teamValues[i].load("control", "TeamUpDownValue", "Value", "TeamUpDownText", "Text");
        } else {
            m_teamControls[i].load(1, 0, "control", "MachineUpDownBase", "MachineUpDownBase4",
                    "TeamArrowRight", "ButtonArrowRight4", "TeamArrowLeft", "ButtonArrowLeft4",
                    m_teamValues[i].animator(), 1 << i, 0, 0, true, true);
            m_teamValues[i].load("control", "TeamUpDownValue", "Value4", "TeamUpDownText", "Text");
        }
    }
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < localPlayerCount; i++) {
        m_teamControls[i].setFrontHandler(&m_onTeamControlFront);
        m_teamValues[i].setChangeHandler(&m_onTeamValueChange);
    }
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(3500);

    m_teamCount.reset();
}

void MultiTeamSelectPage::onActivate() {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    u32 maxTeamSize = menuScenario.spMaxTeamSize;
    u32 playerCount = 12;
    u32 teamCount = (playerCount + maxTeamSize - 1) / maxTeamSize;

    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;

    for (size_t i = 0; i < localPlayerCount; i++) {
        if (!m_teamCount || teamCount != *m_teamCount) {
            m_teamControls[i].reconfigure(teamCount, 0);
        }
        m_teamControls[i].selectDefault(i);
        m_teamControls[i].m_enabled = true;
    }

    m_teamCount = teamCount;

    auto *driverModelManager = MenuModelManager::Instance()->driverModelManager();
    for (size_t i = 0; i < localPlayerCount; i++) {
        auto *model = driverModelManager->handle(i)->model;
        model->setAnim(i, 0);
    }

    m_replacement = PageId::None;
}

void MultiTeamSelectPage::beforeInAnim() {
    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    for (size_t i = 0; i < localPlayerCount; i++) {
        if (anim() == Anim::Next) {
            m_driverModels[i].onPageChange(PageId::CharacterSelect);
        } else {
            m_driverModels[i].onPageChange(PageId::TeamConfirm);
        }
    }
}

void MultiTeamSelectPage::beforeOutAnim() {
    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    for (size_t i = 0; i < localPlayerCount; i++) {
        if (anim() == Anim::Next) {
            m_driverModels[i].onPageChange(PageId::TeamConfirm);
        } else {
            m_driverModels[i].onPageChange(PageId::CharacterSelect);
        }
    }
}

void MultiTeamSelectPage::onRefocus() {
    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    auto *driverModelManager = MenuModelManager::Instance()->driverModelManager();
    for (size_t i = 0; i < localPlayerCount; i++) {
        m_teamControls[i].m_enabled = true;
        auto *model = driverModelManager->handle(i)->model;
        model->setAnim(i, 0);
    }
}

void MultiTeamSelectPage::onBack(u32 localPlayerId) {
    if (!m_teamControls[localPlayerId].m_enabled) {
        m_teamControls[localPlayerId].m_enabled = true;

        auto *driverModelManager = MenuModelManager::Instance()->driverModelManager();
        auto *model = driverModelManager->handle(localPlayerId)->model;
        model->setAnim(localPlayerId, 0);
        return;
    }

    if (localPlayerId != 0) {
        return;
    }

    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    auto *driverModelManager = MenuModelManager::Instance()->driverModelManager();
    for (size_t i = 0; i < localPlayerCount; i++) {
        auto *model = driverModelManager->handle(i)->model;
        model->setAnim(i, 0);
    }

    m_replacement = PageId::CharacterSelect;
    startReplace(Anim::Prev, 0.0f);
}

void MultiTeamSelectPage::onTeamControlFront(UpDownControl *control, u32 localPlayerId) {
    auto *driverModelManager = MenuModelManager::Instance()->driverModelManager();
    if (control->m_enabled) {
        control->m_enabled = false;

        auto *model = driverModelManager->handle(localPlayerId)->model;
        model->setAnim(localPlayerId, 1);

        auto *driverSoundManager = Sound::DriverSoundManager::Instance();
        driverSoundManager->playSelectSound(localPlayerId, -1);
    }

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.players[localPlayerId].spTeam = control->chosen();

    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    for (size_t i = 0; i < localPlayerCount; i++) {
        if (m_teamControls[i].m_enabled) {
            return;
        }
    }

    u32 maxTeamSize = menuScenario.spMaxTeamSize;
    u32 playerCount = 12;
    u32 teamCount = (playerCount + maxTeamSize - 1) / maxTeamSize;
    maxTeamSize = (playerCount + teamCount - 1) / teamCount;
    assert(teamCount <= 6);
    u32 teamSizes[6]{};
    for (size_t i = 0; i < localPlayerCount; i++) {
        teamSizes[menuScenario.players[i].spTeam]++;
    }
    for (size_t i = 0; i < teamCount; i++) {
        if (teamSizes[i] > maxTeamSize) {
            auto *section = SectionManager::Instance()->currentSection();
            auto *messagePagePopup = section->page<PageId::MessagePopup>();
            messagePagePopup->reset();
            messagePagePopup->setWindowMessage(10274);
            push(PageId::MessagePopup, Anim::Next);
            return;
        }
    }
    for (u32 i = localPlayerCount; i < playerCount; i++) {
        do {
            menuScenario.players[i].spTeam = hydro_random_uniform(teamCount);
        } while (teamSizes[menuScenario.players[i].spTeam] >= maxTeamSize);
        teamSizes[menuScenario.players[i].spTeam]++;
    }

    m_replacement = PageId::TeamConfirm;
    f32 delay = std::max(m_teamValues[localPlayerId].getDelay(), driverModelManager->getDelay());
    startReplace(Anim::Next, delay);
}

void MultiTeamSelectPage::onTeamValueChange(TextUpDownValueControl::TextControl *text, u32 index) {
    text->setMessageAll(10268 + index);
    char flagPane[0x20];
    snprintf(flagPane, std::size(flagPane), "flag_%u", index);
    text->setPicture("flag_set_p", flagPane);
}

void MultiTeamSelectPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *context = SectionManager::Instance()->globalContext();
    u32 localPlayerCount = context->m_localPlayerCount;
    auto *driverModelManager = MenuModelManager::Instance()->driverModelManager();
    for (size_t i = 0; i < localPlayerCount; i++) {
        auto *model = driverModelManager->handle(i)->model;
        model->setAnim(i, 0);
    }

    m_replacement = PageId::CharacterSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

} // namespace UI
