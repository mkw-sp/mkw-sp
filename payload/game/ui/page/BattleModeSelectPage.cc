#include "BattleModeSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/SettingsPage.hh"

namespace UI {

void BattleModeSelectPage::onInit() {
    auto section = SectionManager::Instance()->currentSection();
    section->loadTHP();

    REPLACED(onInit)();
}

void BattleModeSelectPage::onButtonFront(const PushButton *button) {
    if (button->m_index == -100) {
        onBackButtonFront(button);
        return;
    }

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    switch (button->m_index) {
    case 0: // Balloon battle
        menuScenario.battleType = 0;
        break;
    case 1: // Coin runners
        menuScenario.battleType = 1;
        break;
    case 2: // Settings
        auto *section = SectionManager::Instance()->currentSection();
        auto *settingsPage = section->page<PageId::MenuSettings>();

        settingsPage->configure(nullptr, PageId::BattleModeSelect);

        startReplace(PageId::MenuSettings, button);
        return;
    }

    startReplace(PageId::CharacterSelect, button);
}

} // namespace UI
