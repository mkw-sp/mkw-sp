#include "eggSystem.hh"

#include <game/ui/SectionManager.hh>
#include <sp/Commands.h>
#include <sp/SaveStateManager.hh>

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

namespace EGG {

void *TSystem::mem1ArenaLo() const {
    return m_mem1ArenaLo;
}

void *TSystem::mem1ArenaHi() const {
    return m_mem1ArenaHi;
}

void *TSystem::mem2ArenaLo() const {
    return m_mem2ArenaLo;
}

void *TSystem::mem2ArenaHi() const {
    return m_mem2ArenaHi;
}

XfbManager *TSystem::xfbManager() {
    return m_xfbManager;
}

TSystem *TSystem::Instance() {
    return &s_instance;
}

} // namespace EGG
