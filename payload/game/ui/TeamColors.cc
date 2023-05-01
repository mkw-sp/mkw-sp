#include "TeamColors.hh"

#include "game/system/SaveManager.hh"

namespace UI::TeamColors {

GXColor Get(u32 teamId) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ColorPalette>();
    bool colorblind = setting == SP::ClientSettings::ColorPalette::Colorblind;

    GXColor colors[12] = {
            {255, 0, 0, 255},
            {0, 170, 255, 255},
            {0, 255, 0, 255},
            {255, 255, 0, 255},
            {255, 0, 255, 255},
            {255, 255, 255, 255},
            {213, 94, 0, 255},
            {0, 114, 178, 255},
            {0, 158, 115, 255},
            {240, 228, 66, 255},
            {204, 121, 167, 255},
            {255, 255, 255, 255},
    };
    return colors[colorblind * 6 + teamId];
}

} // namespace UI::TeamColors
