#include "sp/SaveStateManager.hh"

#include <game/system/SaveManager.hh>
#include <game/ui/SectionManager.hh>
extern "C" {
#include <sp/Commands.h>
}

#include <cstring>

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

    auto sectionId = static_cast<UI::SectionId>(nextSectionId);
    auto anim = nextSectionId < 0 ? UI::Page::Anim::Prev : UI::Page::Anim::Next;

    sectionManager->setNextSection(sectionId, anim);
    sectionManager->startChangeSection(5, 0xff);
}
