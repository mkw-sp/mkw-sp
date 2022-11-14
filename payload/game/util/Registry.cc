#include "Registry.hh"

#include "game/system/SaveManager.hh"

namespace Registry {

const char *getItemPane(u32 itemId, u32 count) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();

    // Return mega TC pane
    if (itemId == 0x0E && setting == SP::ClientSettings::VSMegaClouds::Enable) {
        return "mega_c";
    }

    return REPLACED(getItemPane)(itemId, count);
}

} // namespace Registry
