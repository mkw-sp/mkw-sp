#include "RegionLineColor.hh"

#include "game/host_system/SystemManager.hh"

namespace SP::RegionLineColor {

u32 Get(SP::ClientSettings::RegionLineColor regionLineColorSetting) {
    if (regionLineColorSetting == SP::ClientSettings::RegionLineColor::Default) {
        return System::SystemManager::Instance()->matchingArea();
    } else {
        return static_cast<u32>(regionLineColorSetting);
    }
}

} // namespace SP::RegionLineColor
