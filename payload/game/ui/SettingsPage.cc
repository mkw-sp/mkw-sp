#include "SettingsPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
extern "C" {
#include "game/ui/SettingsPage.h"
}
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

SettingsPage::SettingsPage() = default;

SettingsPage::~SettingsPage() = default;

PageId SettingsPage::getReplacement() {
    return m_replacement;
}

void SettingsPage::onInit() {
    auto sectionId = SectionManager::Instance()->currentSection()->id();

    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(4 + std::size(m_settingControls));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_categoryControl, 0);
    insertChild(2, &m_instructionText, 0);
    insertChild(3, &m_backButton, 0);
    for (u32 i = 0; i < std::size(m_settingControls); i++) {
        insertChild(4 + i, &m_settingControls[i], 0);
    }

    m_pageTitleText.load(false);
    m_categoryControl.load(magic_enum::enum_count<SP::ClientSettings::Category>(), 0, "control",
            "CategoryUpDownBase", "Category", "CategoryUpDownButtonR", "RightButton",
            "CategoryUpDownButtonL", "LeftButton", m_categoryValue.animator(), 0x1, false, false,
            true, true);
    m_categoryValue.load("ranking", "CategoryUpDownValue", "Value", "CategoryUpDownText", "Text");
    for (u32 i = 0; i < std::size(m_settingControls); i++) {
        char variant[0x20];
        snprintf(variant, sizeof(variant), "UpDown%lu", i);
        m_settingControls[i].load(1, 0, "control", "SettingUpDownBase", variant,
                "VSSettingUpDownButtonR", "RightButton", "VSSettingUpDownButtonL", "LeftButton",
                m_settingValues[i].animator(), 0x1, false, false, true, true);
        m_settingValues[i].load("ranking", "SettingUpDownValue", "Value", "SettingUpDownText",
                "Text");
    }
    m_instructionText.load();
    if (Section::GetSceneId(sectionId) == 2 /* Race */) {
        m_backButton.load("message_window", "Back", "ButtonBack", 0x1, false, true);
    } else {
        m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);
    }

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_categoryControl.setFrontHandler(&m_onCategoryControlFront);
    m_categoryControl.setSelectHandler(&m_onCategoryControlSelect);
    m_categoryValue.setChangeHandler(&m_onCategoryValueChange);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    for (u32 i = 0; i < std::size(m_settingControls); i++) {
        m_settingControls[i].setChangeHandler(&m_onSettingControlChange);
        m_settingControls[i].setFrontHandler(&m_onSettingControlFront);
        m_settingControls[i].setSelectHandler(&m_onSettingControlSelect);
    }

    m_pageTitleText.setMessage(10076);

    if (sectionId == SectionId::LicenseSettings) {
        m_replacement = PageId::LicenseSettings;
    } else {
        m_replacement = PageId::None; // Failsafe
    }
}

void SettingsPage::onDeinit() {
    SectionManager::Instance()->saveManagerProxy()->markLicensesDirty();
}

void SettingsPage::onActivate() {
    m_categoryControl.selectDefault(0);

    auto *section = SectionManager::Instance()->currentSection();
    auto *raceMenuPage = section->page(m_replacement)->downcast<RaceMenuPage>();
    if (raceMenuPage != nullptr) {
        raceMenuPage->setReplacement(PageId::None);
    }
}

void SettingsPage::onBack([[maybe_unused]] u32 localPlayerId) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsPage::onCategoryControlFront([[maybe_unused]] UpDownControl *control,
        [[maybe_unused]] u32 localPlayerId) {
    m_settingControls[0].select(localPlayerId);
}

void SettingsPage::onCategoryControlSelect([[maybe_unused]] UpDownControl *control,
        [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(0);
}

void SettingsPage::onCategoryValueChange([[maybe_unused]] TextUpDownValueControl::TextControl *text,
        [[maybe_unused]] u32 index) {
    text->setMessageAll(SP::ClientSettings::categoryMessageIds[index]);

    auto category = static_cast<SP::ClientSettings::Category>(index);
    u32 i = 0;
    for (u32 j = 0; j < SP::ClientSettings::entryCount && i < std::size(m_settingControls); j++) {
        if (SP::ClientSettings::entries[j].category != category) {
            continue;
        }
        if (SP::ClientSettings::entries[j].valueCount == 0) {
            continue;
        }
        // Already configurable in its dedicated page
        if (static_cast<SP::ClientSettings::Setting>(j) == SP::ClientSettings::Setting::DriftMode) {
            continue;
        }
        m_settingControls[i].m_id = j << 16 | i;
        const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[j];
        m_settingControls[i].setMessageAll(entry.messageId);
        u32 count = entry.valueCount;
        u32 chosen = System::SaveManager::Instance()->getSetting(j);
        m_settingControls[i].reconfigure(count, chosen);
        m_settingControls[i].setVisible(true);
        m_settingControls[i].setPlayerFlags(0x1);
        auto *shownText = m_settingValues[i].shownText();
        shownText->setMessageAll(entry.valueMessageIds[chosen]);
        auto *hiddenText = m_settingValues[i].hiddenText();
        hiddenText->setMessageAll(entry.valueMessageIds[chosen]);
        i++;
    }
    for (; i < std::size(m_settingControls); i++) {
        m_settingControls[i].setVisible(false);
        m_settingControls[i].setPlayerFlags(0x0);
    }
}

void SettingsPage::onSettingControlChange([[maybe_unused]] UpDownControl *control,
        [[maybe_unused]] u32 localPlayerId, [[maybe_unused]] u32 index) {
    auto *text = m_settingValues[control->m_id & 0xffff].shownText();
    const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[control->m_id >> 16];
    text->setMessageAll(entry.valueMessageIds[index]);
    m_instructionText.setMessage(entry.valueExplanationMessageIds[index]);
    System::SaveManager::Instance()->setSetting(control->m_id >> 16, index);
}

void SettingsPage::onSettingControlFront([[maybe_unused]] UpDownControl *control,
        [[maybe_unused]] u32 localPlayerId) {
    u32 index = control->m_id & 0xffff;
    if (index + 1 == std::size(m_settingControls) || !m_settingControls[index + 1].getVisible()) {
        m_categoryControl.select(localPlayerId);
    } else {
        m_settingControls[index + 1].select(localPlayerId);
    }
}

void SettingsPage::onSettingControlSelect([[maybe_unused]] UpDownControl *control,
        [[maybe_unused]] u32 localPlayerId) {
    u32 chosen = control->chosen();
    const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[control->m_id >> 16];
    m_instructionText.setMessage(entry.valueExplanationMessageIds[chosen]);
    System::SaveManager::Instance()->setSetting(control->m_id >> 16, chosen);
}

void SettingsPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

} // namespace UI

extern "C" {
void SettingsPage_SetReplacement(s32 pageId) {
    auto *section = UI::SectionManager::Instance()->currentSection();
    section->page<UI::PageId::Settings>()->m_replacement = static_cast<UI::PageId>(pageId);
}
}
