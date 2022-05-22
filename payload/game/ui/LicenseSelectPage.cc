#include "LicenseSelectPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <cstdio>
#include <new>

namespace UI {

LicenseSelectPage::LicenseSelectPage() = default;

LicenseSelectPage::~LicenseSelectPage() = default;

PageId LicenseSelectPage::getReplacement() {
    return m_replacement;
}

void LicenseSelectPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(3 + m_licenseButtons.size());
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_servicePackButton, 0);
    insertChild(2, &m_backButton, 0);
    for (size_t i = 0; i < m_licenseButtons.size(); i++) {
        insertChild(3 + i, &m_licenseButtons[i], 0);
    }

    m_pageTitleText.load(false);
    m_servicePackButton.load("button", "LicenseSelectS", "Option", 0x1, false, false);
    for (size_t i = 0; i < m_licenseButtons.size(); i++) {
        char variant[0x10];
        snprintf(variant, sizeof(variant), "License%zu", i);
        m_licenseButtons[i].load("button", "LicenseSelect", variant, 0x1, false, false);
    }
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_servicePackButton.setFrontHandler(&m_onServicePackButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_miiGroup.init(4, 0x1, nullptr);
    m_pageTitleText.setMessage(2104, nullptr);
    m_servicePackButton.setMessageAll(10083, nullptr);
    for (size_t i = 0; i < m_licenseButtons.size(); i++) {
        PushButton &button = m_licenseButtons[i];
        const System::SaveManager *saveManager = System::SaveManager::Instance();
        if (i < saveManager->spLicenseCount()) {
            button.setPaneVisible("new", false);
            System::MiiId miiId = saveManager->getSpLicenseMiiId(i);
            m_miiGroup.insertFromId(i, &miiId);
            button.setPaneVisible("mii", true);
            button.setMiiPicture("mii", &m_miiGroup, i, 0);
            MessageInfo info{};
            info.miis[0] = m_miiGroup.get(i);
            button.setMessage("player", 9501, &info);
        } else if (i == saveManager->spLicenseCount()) {
            button.setPaneVisible("new", true);
            button.setMessage("new", 6017, nullptr);
            button.setPaneVisible("mii", false);
        } else {
            button.setVisible(false);
            button.setPlayerFlags(0x0);
        }
        button.m_index = i;
        button.setFrontHandler(&m_onLicenseButtonFront, false);
    }
}

void LicenseSelectPage::onActivate() {
    u32 index = System::SaveManager::Instance()->spCurrentLicense().value_or(0);
    m_licenseButtons[index].selectDefault(0);

    m_replacement = PageId::None;
}

void LicenseSelectPage::onBack(u32 UNUSED(localPlayerId)) {
    System::SaveManager::Instance()->unselectSpLicense();

    m_replacement = PageId::Title;
    startReplace(Animation::Prev, 0.0f);
}

void LicenseSelectPage::onServicePackButtonFront(PushButton *button, u32 UNUSED(localPlayerId)) {
    f32 delay = button->getDelay();
    changeSection(SectionId::Channel, Animation::Next, delay);
}

void LicenseSelectPage::onLicenseButtonFront(PushButton *button, u32 UNUSED(localPlayerId)) {
    System::SaveManager *saveManager = System::SaveManager::Instance();
    SectionManager *sectionManager = SectionManager::Instance();
    GlobalContext *globalContext = sectionManager->globalContext();
    globalContext->onChangeLicense();

    u32 index = button->m_index;
    if (index < saveManager->spLicenseCount()) {
        saveManager->selectSpLicense(index);
        const System::Mii *mii = m_miiGroup.get(index);
        if (mii) {
            saveManager->createLicense(0, &mii->id, mii->name);
            saveManager->selectLicense(0);
            globalContext->m_localPlayerMiis.copy(&m_miiGroup, index, 0);
            m_replacement = PageId::TopMenu;
            f32 delay = button->getDelay();
            startReplace(Animation::Next, delay);
        } else {
            ConfirmPage *confirmPage = sectionManager->currentSection()->page<ConfirmPage>();
            confirmPage->reset();
            confirmPage->setTitleMessage(2200, nullptr);
            confirmPage->setWindowMessage(2205, nullptr);
            confirmPage->m_confirmHandler = &m_onChangeConfirm;
            confirmPage->m_cancelHandler = &m_onCancel;
            m_replacement = PageId::Confirm;
            f32 delay = button->getDelay();
            startReplace(Animation::Next, delay);
        }
    } else if (index == saveManager->spLicenseCount()) {
        saveManager->unselectSpLicense();
        ConfirmPage *confirmPage = sectionManager->currentSection()->page<ConfirmPage>();
        confirmPage->reset();
        confirmPage->setTitleMessage(2102, nullptr);
        confirmPage->setWindowMessage(2101, nullptr);
        confirmPage->m_confirmHandler = &m_onCreateConfirm;
        confirmPage->m_cancelHandler = &m_onCancel;
        m_replacement = PageId::Confirm;
        f32 delay = button->getDelay();
        startReplace(Animation::Next, delay);
    }
}

void LicenseSelectPage::onBackButtonFront(PushButton *button, u32 UNUSED(localPlayerId)) {
    System::SaveManager::Instance()->unselectSpLicense();

    m_replacement = PageId::Title;
    f32 delay = button->getDelay();
    startReplace(Animation::Prev, delay);
}

void LicenseSelectPage::onCreateConfirm(ConfirmPage *UNUSED(confirmPage), f32 delay) {
    SectionManager *sectionManager = SectionManager::Instance();
    sectionManager->setNextSection(SectionId::MiiSelectCreate, Animation::Next);
    sectionManager->startChangeSection(delay, 0x000000ff);
}

void LicenseSelectPage::onChangeConfirm(ConfirmPage *UNUSED(confirmPage), f32 delay) {
    SectionManager *sectionManager = SectionManager::Instance();
    sectionManager->setNextSection(SectionId::MiiSelectChange, Animation::Next);
    sectionManager->startChangeSection(delay, 0x000000ff);
}

void LicenseSelectPage::onCancel(ConfirmPage *confirmPage, f32 UNUSED(delay)) {
    confirmPage->m_replacement = PageId::LicenseSelect;
}

} // namespace UI

extern "C" void *my_LicenseSelectPage_ct(void *self) {
    return new (self) UI::LicenseSelectPage();
}

static_assert(sizeof_LicenseSelectPage == sizeof(UI::LicenseSelectPage));
