#include "Registry.hh"

#include "game/system/SaveManager.hh"

    namespace Registry {

        const char* getItemPane(u32 itemId, u32 count) {
            auto* saveManager = System::SaveManager::Instance();
            auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();

            // If the item is a thundercloud, and the option for mega tcs is enabled, return the mega tc
            // pane, otehrwise go back to original function
            if (itemId == 0x0E) {
                if (setting == SP::ClientSettings::VSMegaClouds::Enable) {
                    return "mega_c";
                }
            }
            
            return REPLACED(getItemPane)(itemId, count);
            
        }

    } // namespace Registry