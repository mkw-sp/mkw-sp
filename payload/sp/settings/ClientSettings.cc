#include "ClientSettings.hh"

#include <iterator>

namespace SP::ClientSettings {

const char name[] = "MKW-SP Settings";

const char *categoryNames[] = {
    [static_cast<u32>(Category::Race)] = "Race",
    [static_cast<u32>(Category::TA)] = "TTs",
    [static_cast<u32>(Category::License)] = "License",
};

static const char *driftModeValueNames[] = {
    [static_cast<u32>(DriftMode::Manual)] = "Manual",
    [static_cast<u32>(DriftMode::Auto)] = "Auto",
};

static const char *hudLabelsValueNames[] = {
    [static_cast<u32>(HUDLabels::Hide)] = "Hide",
    [static_cast<u32>(HUDLabels::Show)] = "Show",
};

static const char *fov169ValueNames[] = {
    [static_cast<u32>(FOV169::FOV169)] = "16:9",
    [static_cast<u32>(FOV169::FOV43)] = "4:3",
};

static const char *mapIconsValueNames[] = {
    [static_cast<u32>(MapIcons::Characters)] = "Characters",
    [static_cast<u32>(MapIcons::Miis)] = "Miis",
};

static const char *inputDisplayValueNames[] = {
    [static_cast<u32>(InputDisplay::Disable)] = "None",
    [static_cast<u32>(InputDisplay::Simple)] = "Simple",
};

static const char *taRuleClassValueNames[] = {
    [static_cast<u32>(TARuleClass::CC150)] = "150cc",
    [static_cast<u32>(TARuleClass::CC200)] = "200cc",
};

static const char *taRuleGhostSortingValueNames[] = {
    [static_cast<u32>(TARuleGhostSorting::Fastest)] = "Fastest",
    [static_cast<u32>(TARuleGhostSorting::Slowest)] = "Slowest",
    [static_cast<u32>(TARuleGhostSorting::Newest)] = "Newest",
    [static_cast<u32>(TARuleGhostSorting::Oldest)] = "Oldest",
};

static const char *taRuleGhostTagVisibilityValueNames[] = {
    [static_cast<u32>(TARuleGhostTagVisibility::None)] = "None",
    [static_cast<u32>(TARuleGhostTagVisibility::Watched)] = "Watched",
    [static_cast<u32>(TARuleGhostTagVisibility::All)] = "All",
};

static const char *taRuleGhostTagContentValueNames[] = {
    [static_cast<u32>(TARuleGhostTagContent::Name)] = "Name",
    [static_cast<u32>(TARuleGhostTagContent::Time)] = "Time",
    [static_cast<u32>(TARuleGhostTagContent::TimeNoLeading)] = "TimeNoLeading",
    [static_cast<u32>(TARuleGhostTagContent::Date)] = "Date",
};

static const char *taRuleSolidGhostsValueNames[] = {
    [static_cast<u32>(TARuleSolidGhosts::None)] = "None",
    [static_cast<u32>(TARuleSolidGhosts::Watched)] = "Watched",
    [static_cast<u32>(TARuleSolidGhosts::All)] = "All",
};

static const char *taRuleGhostSoundValueNames[] = {
    [static_cast<u32>(TARuleGhostSound::None)] = "None",
    [static_cast<u32>(TARuleGhostSound::Watched)] = "Watched",
    [static_cast<u32>(TARuleGhostSound::All)] = "All",
};

static const char *pageTransitionsValueNames[] = {
    [static_cast<u32>(PageTransitions::Disable)] = "Disable",
    [static_cast<u32>(PageTransitions::Enable)] = "Enable",
};

const Entry entries[] = {
    [static_cast<u32>(Setting::DriftMode)] = {
        .category = Category::Race,
        .name = "DriftMode",
        .defaultValue = static_cast<u32>(DriftMode::Default),
        .valueCount = std::size(driftModeValueNames),
        .valueNames = driftModeValueNames,
    },
    [static_cast<u32>(Setting::HUDLabels)] = {
        .category = Category::Race,
        .name = "HUDLabels",
        .defaultValue = static_cast<u32>(HUDLabels::Default),
        .valueCount = std::size(hudLabelsValueNames),
        .valueNames = hudLabelsValueNames,
    },
    [static_cast<u32>(Setting::FOV169)] = {
        .category = Category::Race,
        .name = "FOV169",
        .defaultValue = static_cast<u32>(FOV169::Default),
        .valueCount = std::size(fov169ValueNames),
        .valueNames = fov169ValueNames,
    },
    [static_cast<u32>(Setting::MapIcons)] = {
        .category = Category::Race,
        .name = "MapIcons",
        .defaultValue = static_cast<u32>(DriftMode::Default),
        .valueCount = std::size(mapIconsValueNames),
        .valueNames = mapIconsValueNames,
    },
    [static_cast<u32>(Setting::InputDisplay)] = {
        .category = Category::Race,
        .name = "InputDisplay",
        .defaultValue = static_cast<u32>(InputDisplay::Default),
        .valueCount = std::size(inputDisplayValueNames),
        .valueNames = inputDisplayValueNames,
    },
    [static_cast<u32>(Setting::TARuleClass)] = {
        .category = Category::TA,
        .name = "Class",
        .defaultValue = static_cast<u32>(TARuleClass::Default),
        .valueCount = std::size(taRuleClassValueNames),
        .valueNames = taRuleClassValueNames,
    },
    [static_cast<u32>(Setting::TARuleGhostSorting)] = {
        .category = Category::TA,
        .name = "GhostSorting",
        .defaultValue = static_cast<u32>(TARuleGhostSorting::Default),
        .valueCount = std::size(taRuleGhostSortingValueNames),
        .valueNames = taRuleGhostSortingValueNames,
    },
    [static_cast<u32>(Setting::TARuleGhostTagVisibility)] = {
        .category = Category::TA,
        .name = "GhostTagVisibility",
        .defaultValue = static_cast<u32>(TARuleGhostTagVisibility::Default),
        .valueCount = std::size(taRuleGhostTagVisibilityValueNames),
        .valueNames = taRuleGhostTagVisibilityValueNames,
    },
    [static_cast<u32>(Setting::TARuleGhostTagContent)] = {
        .category = Category::TA,
        .name = "GhostTagContent",
        .defaultValue = static_cast<u32>(TARuleGhostTagContent::Default),
        .valueCount = std::size(taRuleGhostTagContentValueNames),
        .valueNames = taRuleGhostTagContentValueNames,
    },
    [static_cast<u32>(Setting::TARuleSolidGhosts)] = {
        .category = Category::TA,
        .name = "SolidGhosts",
        .defaultValue = static_cast<u32>(TARuleSolidGhosts::Default),
        .valueCount = std::size(taRuleSolidGhostsValueNames),
        .valueNames = taRuleSolidGhostsValueNames,
    },
    [static_cast<u32>(Setting::TARuleGhostSound)] = {
        .category = Category::TA,
        .name = "GhostSound",
        .defaultValue = static_cast<u32>(TARuleGhostSound::Default),
        .valueCount = std::size(taRuleGhostSoundValueNames),
        .valueNames = taRuleGhostSoundValueNames,
    },
    [static_cast<u32>(Setting::MiiAvatar)] = {
        .category = Category::License,
        .name = "MiiAvatar",
        .defaultValue = 0x80000001,
        .valueCount = 0,
        .valueNames = nullptr,
    },
    [static_cast<u32>(Setting::MiiClient)] = {
        .category = Category::License,
        .name = "MiiClient",
        .defaultValue = 0xECFF82D2,
        .valueCount = 0,
        .valueNames = nullptr,
    },
    [static_cast<u32>(Setting::LoadingScreenColor)] = {
        .category = Category::License,
        .name = "LoadingScreenColor",
        .defaultValue = 0xFF,
        .valueCount = 0,
        .valueNames = nullptr,
    },
    [static_cast<u32>(Setting::PageTransitions)] = {
        .category = Category::License,
        .name = "PageTransitions",
        .defaultValue = static_cast<u32>(PageTransitions::Default),
        .valueCount = std::size(pageTransitionsValueNames),
        .valueNames = pageTransitionsValueNames,
    },
};

} // namespace SP::ClientSettings
