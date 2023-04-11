#include "sp/SaveStateManager.hh"

#include <game/system/SaveManager.hh>
#include <game/ui/SectionManager.hh>
extern "C" {
#include <sp/Commands.h>
}

#include <cstring>

sp_define_command("/example_command", "Example command", const char *tmp) {
    (void)tmp;
}

sp_define_command("/ui_info", "Dump information about the UI state to the console",
        const char *tmp) {
    (void)tmp;

    auto sectionManager = UI::SectionManager::Instance();
    auto section = sectionManager->currentSection();
    section->logDebuggingInfo(!strcmp(tmp, "/ui_info v"));
}

sp_define_command("/store", "", const char *tmp) {
    (void)tmp;

    if (auto saveStateManager = SP::SaveStateManager::Instance()) {
        saveStateManager->save();
    } else {
        OSReport("SaveStateManager not initialized\n");
    }
}

sp_define_command("/reload", "", const char *tmp) {
    (void)tmp;

    if (auto saveStateManager = SP::SaveStateManager::Instance()) {
        saveStateManager->reload();
    } else {
        OSReport("SaveStateManager not initialized\n");
    }
}

sp_define_command("/section", "Transition to a certain game section", const char *tmp) {
    auto *sectionManager = UI::SectionManager::Instance();
    if (sectionManager == nullptr) {
        OSReport("&aError: Section manager unavailable\n");
        return;
    }

    int nextSectionId = 0;
    if (!sscanf(tmp, "/section %i", &nextSectionId)) {
        OSReport("&aUsage /section <id>\n");
        return;
    }

    OSReport("&aSwitching to section %d\n", nextSectionId);

    auto *saveManager = System::SaveManager::Instance();
    if (saveManager == nullptr) {
        OSReport("&aError: Save manager unavailable\n");
        return;
    }

    // Default to license 0
    saveManager->selectSPLicense(0);
    // TODO create base license
    saveManager->selectLicense(0);

    auto sectionId = static_cast<UI::SectionId>(abs(nextSectionId));
    auto anim = nextSectionId < 0 ? UI::Page::Anim::Prev : UI::Page::Anim::Next;

    sectionManager->setNextSection(sectionId, anim);
    sectionManager->startChangeSection(5, 0xff);
}

sp_define_command("/set", "Sets a .ini setting key-value", const char *tmp) {
    auto *saveManager = System::SaveManager::Instance();
    if (saveManager == nullptr) {
        OSReport("set: Failed to load Save Manager\n");
        return;
    }

    char setting[64];
    char value[64];
    if (2 != sscanf(tmp, "/set %63s %63s", setting, value)) {
        OSReport("&a/set: Invalid arguments\n");
        return;
    }

    if (!saveManager->spCurrentLicense().has_value()) {
        OSReport("&a/set: No license active\n");
        return;
    }

    saveManager->setSetting(setting, value);
}

sp_define_command("/instant_menu", "Toggle instant menu transitions", const char *tmp) {
    (void)tmp;

    auto *saveManager = System::SaveManager::Instance();
    if (saveManager == nullptr) {
        OSReport("instant_menu: Failed to load Save Manager\n");
        return;
    }

    auto oldValue = saveManager->getSetting<SP::ClientSettings::Setting::PageTransitions>();
    auto newValue = oldValue == SP::ClientSettings::PageTransitions::Enable ?
            SP::ClientSettings::PageTransitions::Disable :
            SP::ClientSettings::PageTransitions::Enable;

    saveManager->setSetting<SP::ClientSettings::Setting::PageTransitions>(newValue);
    if (newValue == SP::ClientSettings::PageTransitions::Enable) {
        OSReport("instant_menu: Menu transition animations: Enabled\n");
    } else {
        OSReport("instant_menu: Menu transition animations: Disabled\n");
    }
}
