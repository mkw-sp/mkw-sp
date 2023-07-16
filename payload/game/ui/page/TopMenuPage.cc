#include "TopMenuPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <features/online/Online.hh>

#define MAX_SP_LICENSE_COUNT 6

namespace UI {

extern "C" {

void TopMenuPage_initMiiGroup(TopMenuPage *self) {
    return self->initMiiGroup();
}

void TopMenuPage_refreshFileAdminButton(TopMenuPage *self) {
    return self->refreshFileAdminButton();
}

} // extern "C"

void TopMenuPage::onInit() {
#if !ENABLE_ONLINE
    s_buttonNames[3] = "TopMenuChannelWakuNoOnline";
    m_buttonCount = 3;
#else
    m_buttonCount = 4;
#endif

    REPLACED(onInit)();
}

void TopMenuPage::vf_88(int buttonIndex) {
#if !ENABLE_ONLINE
    if (buttonIndex == 2) {
        buttonIndex = 3;
    }
#endif

    REPLACED(vf_88)(buttonIndex);
}

void TopMenuPage::initMiiGroup() {
    m_miiGroup = new MiiGroup;
    m_miiGroup->init(MAX_SP_LICENSE_COUNT, 0x4, nullptr);

    auto *saveManager = System::SaveManager::Instance();
    for (u32 i = 0; i < saveManager->spLicenseCount(); i++) {
        auto miiId = saveManager->getMiiId(i);
        m_miiGroup->insertFromId(i, &miiId);
    }
}

void TopMenuPage::refreshFileAdminButton() {
    auto *saveManager = System::SaveManager::Instance();

    auto index = *saveManager->spCurrentLicense();
    m_fileAdminButton->setMiiPicture("mii", m_miiGroup, index, 2);
}

void TopMenuPage::onButtonSelect(PushButton *button) {
    if (button->m_index == -100) {
        m_instructionText->setMessage(0, nullptr);
    } else {
        const u32 messageIds[] = {
                10073,
                2021,
                2022,
                2023,
                20001,
                20002,
                2026,
                10074,
        };

        m_instructionText->setMessage(messageIds[button->m_index], nullptr);
    }
}

void TopMenuPage::onButtonFront(PushButton *button) {
    if (button->m_index == 6) {
        auto *sectionManager = SectionManager::Instance();
        if (!sectionManager->saveManagerProxy()->savingDisabled()) {
            initRaceConfig(1);

            auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
            menuScenario.gameMode = System::RaceConfig::GameMode::TimeAttack;
            sectionManager->globalContext()->m_currentPack = 0;

            requestChangeSection(SectionId::ServicePackChannel, button);
        }
    }

    REPLACED(onButtonFront)(button);
}

} // namespace UI
