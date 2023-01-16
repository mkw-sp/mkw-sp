#include "Registry.hh"

#include "game/system/SaveManager.hh"

namespace Registry {

// This disables trying to load red/blue thumbnails and models
// for all the vehicles in the vehicle select screen
u32 UseBattleRenders() {
    return false;
}

const char *GetItemPane(u32 itemId, u32 count) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();

    // Return mega TC pane
    if (itemId == 0x0E && setting == SP::ClientSettings::VSMegaClouds::Enable) {
        return "mega_c";
    }

    return REPLACED(GetItemPane)(itemId, count);
}

} // namespace Registry
