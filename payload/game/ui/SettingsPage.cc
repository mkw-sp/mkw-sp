#include "SettingsPage.hh"

#include "game/host_system/Scene.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

SettingsPage::SettingsPage() = default;

SettingsPage::~SettingsPage() = default;

void SettingsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(4 + std::size(m_settingControls) + !!blackBack());
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_categoryControl, 0);
    insertChild(2, instructionText(), 0);
    insertChild(3, &m_backButton, 0);
    for (u32 i = 0; i < std::size(m_settingControls); i++) {
        insertChild(4 + i, &m_settingControls[i], 0);
    }
    if (blackBack()) {
        insertChild(4 + std::size(m_settingControls), blackBack(), 0);
    }

    m_pageTitleText.load(false);
    m_categoryControl.load(getSheetCount(), 0, "control", "CategoryUpDownBase", "Category",
            "CategoryUpDownButtonR", "RightButton", "CategoryUpDownButtonL", "LeftButton",
            m_categoryValue.animator(), 0x1, false, false, true, true);
    m_categoryValue.load("ranking", "CategoryUpDownValue", "Value", "CategoryUpDownText", "Text");
    for (u32 i = 0; i < std::size(m_settingControls); i++) {
        char variant[0x20];
        snprintf(variant, sizeof(variant), "UpDown%u", i);
        m_settingControls[i].load(1, 0, "control", "SettingUpDownBase", variant,
                "SettingUpDownButtonR", "RightButton", "SettingUpDownButtonL", "LeftButton",
                m_settingValues[i].animator(), 0x1, false, false, true, true);
        m_settingValues[i].load("ranking", "SettingUpDownValue", "Value", "SettingUpDownText",
                "Text");
    }
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (blackBack()) {
        instructionText()->load("bg", "ObiInstructionTextPopup", "ObiInstructionTextPopup",
                nullptr);
        m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);
    } else if (Section::GetSceneId(sectionId) == System::SceneId::Race) {
        m_backButton.load("message_window", "Back", "ButtonBack", 0x1, false, true);
        instructionText()->load("bg", "RaceObiInstructionText", "RaceObiInstructionText", nullptr);
    } else {
        m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);
        instructionText()->load("bg", "MenuObiInstructionText", "MenuObiInstructionText", nullptr);
    }
    if (blackBack()) {
        blackBack()->load("message_window", "BlackBack", "BlackBack");
        blackBack()->m_zIndex = -1.0f;
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
}

void SettingsPage::onActivate() {
    instructionText()->setMessageAll(0);

    m_categoryControl.selectDefault(0);
}

BlackBackControl *SettingsPage::blackBack() {
    return nullptr;
}

void SettingsPage::onBack(u32 /* localPlayerId */) {
    if (m_handler) {
        m_handler->handle(this, nullptr);
    }

    startReplace(Anim::Prev, 0.0f);
}

void SettingsPage::onCategoryControlFront(UpDownControl * /* control */, u32 localPlayerId) {
    m_settingControls[0].select(localPlayerId);
}

void SettingsPage::onCategoryControlSelect(UpDownControl * /* control */, u32 /* localPlayerId */) {
    instructionText()->setMessageAll(0);
}

void SettingsPage::onCategoryValueChange(TextUpDownValueControl::TextControl *text, u32 index) {
    auto categoryInfo = getCategoryInfo(index);

    if (categoryInfo.categorySheetCount >= 2) {
        MessageInfo info{};
        info.messageIds[0] = SP::ClientSettings::categoryMessageIds[categoryInfo.categoryIndex];
        info.intVals[0] = categoryInfo.categorySheetIndex + 1;
        info.intVals[1] = categoryInfo.categorySheetCount;
        text->setMessageAll(10182, &info);
    } else {
        text->setMessageAll(SP::ClientSettings::categoryMessageIds[categoryInfo.categoryIndex]);
    }

    auto category = static_cast<SP::ClientSettings::Category>(categoryInfo.categoryIndex);
    u32 i = 0;
    for (u32 j = categoryInfo.settingIndex;
            j < SP::ClientSettings::entryCount && i < std::size(m_settingControls); j++) {
        if (SP::ClientSettings::entries[j].category != category) {
            continue;
        }
        if (SP::ClientSettings::entries[j].valueCount == 0) {
            continue;
        }
        if (SP::ClientSettings::entries[j].hidden) {
            continue;
        }
        m_settingControls[i].m_id = j << 16 | i;
        const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[j];
        m_settingControls[i].setMessageAll(entry.messageId);
        u32 count = entry.valueCount;
        u32 chosen = System::SaveManager::Instance()->getSetting(j) - entry.valueOffset;
        m_settingControls[i].reconfigure(count, chosen);
        m_settingControls[i].setVisible(true);
        m_settingControls[i].setPlayerFlags(0x1);
        auto *shownText = m_settingValues[i].shownText();
        auto *hiddenText = m_settingValues[i].hiddenText();
        if (entry.valueNames) {
            shownText->setMessageAll(entry.valueMessageIds[chosen]);
            hiddenText->setMessageAll(entry.valueMessageIds[chosen]);
        } else {
            chosen += entry.valueOffset;
            MessageInfo info{};
            info.intVals[0] = chosen;
            shownText->setMessageAll(entry.valueMessageIds[0], &info);
            hiddenText->setMessageAll(entry.valueMessageIds[0], &info);
        }
        i++;
    }
    for (; i < std::size(m_settingControls); i++) {
        m_settingControls[i].setVisible(false);
        m_settingControls[i].setPlayerFlags(0x0);
    }
}

void SettingsPage::onSettingControlChange(UpDownControl *control, u32 /* localPlayerId */,
        u32 index) {
    auto *text = m_settingValues[control->m_id & 0xffff].shownText();
    const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[control->m_id >> 16];
    if (entry.valueNames) {
        text->setMessageAll(entry.valueMessageIds[index]);
        instructionText()->setMessageAll(entry.valueExplanationMessageIds[index]);
    } else {
        index += entry.valueOffset;
        MessageInfo info{};
        info.intVals[0] = index;
        text->setMessageAll(entry.valueMessageIds[0], &info);
        instructionText()->setMessageAll(entry.valueExplanationMessageIds[0], &info);
    }
    System::SaveManager::Instance()->setSetting(control->m_id >> 16, index);
    if (control->m_id >> 16 == static_cast<u32>(SP::ClientSettings::Setting::VanillaMode)) {
        System::SPFooter::OnVanilla(index);
    }
}

void SettingsPage::onSettingControlFront(UpDownControl *control, u32 localPlayerId) {
    u32 index = control->m_id & 0xffff;
    if (index + 1 == std::size(m_settingControls) || !m_settingControls[index + 1].getVisible()) {
        m_categoryControl.select(localPlayerId);
    } else {
        m_settingControls[index + 1].select(localPlayerId);
    }
}

void SettingsPage::onSettingControlSelect(UpDownControl *control, u32 /* localPlayerId */) {
    u32 chosen = control->chosen();
    const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[control->m_id >> 16];
    if (entry.valueNames) {
        instructionText()->setMessageAll(entry.valueExplanationMessageIds[chosen]);
    } else {
        chosen += entry.valueOffset;
        MessageInfo info{};
        info.intVals[0] = chosen;
        instructionText()->setMessageAll(entry.valueExplanationMessageIds[0], &info);
    }
}

void SettingsPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    if (m_handler) {
        m_handler->handle(this, button);
    }

    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

u32 SettingsPage::getSheetCount() const {
    u32 sheetCount = 0;
    for (u32 i = 0; i < magic_enum::enum_count<SP::ClientSettings::Category>(); i++) {
        auto category = static_cast<SP::ClientSettings::Category>(i);
        u32 settingCount = 0;
        for (u32 j = 0; j < magic_enum::enum_count<SP::ClientSettings::Setting>(); j++) {
            if (SP::ClientSettings::entries[j].category != category) {
                continue;
            }
            if (SP::ClientSettings::entries[j].valueCount == 0) {
                continue;
            }
            if (SP::ClientSettings::entries[j].hidden) {
                continue;
            }
            settingCount++;
        }
        u32 controlCount = std::size(m_settingControls);
        sheetCount += (settingCount + controlCount - 1) / controlCount;
    }
    return sheetCount;
}

SettingsPage::CategoryInfo SettingsPage::getCategoryInfo(u32 sheetIndex) const {
    u32 controlCount = std::size(m_settingControls);
    CategoryInfo info{};
    u32 sheetCount = 0;
    u32 categoryCount = magic_enum::enum_count<SP::ClientSettings::Category>();
    for (; info.categoryIndex < categoryCount; info.categoryIndex++) {
        auto category = static_cast<SP::ClientSettings::Category>(info.categoryIndex);
        u32 settingCount = 0;
        for (u32 i = 0; i < magic_enum::enum_count<SP::ClientSettings::Setting>(); i++) {
            if (SP::ClientSettings::entries[i].category != category) {
                continue;
            }
            if (SP::ClientSettings::entries[i].hidden) {
                continue;
            }
            if (SP::ClientSettings::entries[i].valueCount == 0) {
                continue;
            }
            if (settingCount % controlCount == 0) {
                if (sheetCount == sheetIndex) {
                    info.categorySheetIndex = settingCount / std::size(m_settingControls);
                    info.settingIndex = i;
                }
                sheetCount++;
            }
            settingCount++;
        }
        if (sheetCount > sheetIndex) {
            info.categorySheetCount = (settingCount + controlCount - 1) / controlCount;
            return info;
        }
    }
    assert(false);
}

SettingsPagePopup::SettingsPagePopup() = default;

SettingsPagePopup::~SettingsPagePopup() = default;

void SettingsPagePopup::onInit() {
    SettingsPage::onInit();

    m_popRequested = false;
}

void SettingsPagePopup::beforeCalc() {
    if (state() == State::State4 && m_popRequested) {
        startReplace(Anim::Prev, 0.0f);
        m_popRequested = false;
    }
}

LayoutUIControl *SettingsPagePopup::instructionText() {
    return &m_instructionText;
}

BlackBackControl *SettingsPagePopup::blackBack() {
    return &m_blackBack;
}

void SettingsPagePopup::configure(IHandler *handler) {
    m_handler = handler;
}

void SettingsPagePopup::pop(Anim anim) {
    setAnim(anim);
    m_popRequested = true;
}

MenuSettingsPage::MenuSettingsPage() = default;

MenuSettingsPage::~MenuSettingsPage() = default;

PageId MenuSettingsPage::getReplacement() {
    return m_replacement;
}

void MenuSettingsPage::onInit() {
    SettingsPage::onInit();

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (!blackBack() && sectionId == SectionId::LicenseSettings) {
        m_replacement = PageId::LicenseSettingsTop;
    } else {
        m_replacement = PageId::None; // Failsafe
    }
}

void MenuSettingsPage::onActivate() {
    SettingsPage::onActivate();

    if (m_replacement != PageId::None) {
        auto *section = SectionManager::Instance()->currentSection();
        auto *raceMenuPage = section->page(m_replacement)->downcast<RaceMenuPage>();
        if (raceMenuPage != nullptr) {
            raceMenuPage->setReplacement(PageId::None);
        }
    }
}

LayoutUIControl *MenuSettingsPage::instructionText() {
    return &m_instructionText;
}

BlackBackControl *MenuSettingsPage::blackBack() {
    return nullptr;
}

void MenuSettingsPage::configure(IHandler *handler, PageId replacement) {
    m_handler = handler;
    m_replacement = replacement;
}

} // namespace UI
