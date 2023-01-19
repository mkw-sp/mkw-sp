#include "game/ui/page/BattleModeSelectPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/system/RaceConfig.hh"

extern "C" {
#include <revolution.h>
#include <sp/StackTrace.h>
}

namespace UI {

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

}
