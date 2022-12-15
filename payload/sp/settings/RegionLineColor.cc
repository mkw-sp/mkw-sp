#include "RegionLineColor.hh"

extern "C" {
#include <game/host_system/SystemManager.h>
}

namespace SP::RegionLineColor {

u32 Get(SP::ClientSettings::RegionLineColor regionLineColorSetting) {
    if (regionLineColorSetting == SP::ClientSettings::RegionLineColor::Default) {
        return s_systemManager->matchingArea;
    } else {
        return static_cast<u32>(regionLineColorSetting);
    }
}

} // namespace SP::RegionLineColor
