#pragma once

#include "sp/settings/Settings.hh"

namespace SP::ClientSettings {

enum class Setting {
    // Race
    DriftMode,
    HUDLabels,
    FOV169,
    MapIcons,
    InputDisplay,

    // TA
    TARuleClass,
    TARuleGhostSorting,
    TARuleGhostTagVisibility,
    TARuleGhostTagContent,
    TARuleSolidGhosts,
    TARuleGhostSound,

    // License
    MiiAvatar,
    MiiClient,
    LoadingScreenColor,
    PageTransitions,

    Max,
};

enum class Category {
    Race,
    TA,
    License,

    Max,
};

enum class DriftMode {
    Manual,
    Auto,

    Max,
    Default = Manual,
};

enum class HUDLabels {
    Hide,
    Show,

    Max,
    Default = Show,
};

enum class FOV169 {
    FOV169,
    FOV43,

    Max,
    Default = FOV169,
};

enum class MapIcons {
    Characters,
    Miis,

    Max,
    Default = Miis,
};

enum class InputDisplay {
    Disable,
    Simple,

    Max,
    Default = Simple,
};

enum class TARuleClass {
    CC150,
    CC200,

    Max,
    Default = CC150,
};

enum class TARuleGhostSorting {
    Fastest,
    Slowest,
    Newest,
    Oldest,

    Max,
    Default = Fastest,
};

enum class TARuleGhostTagVisibility {
    None,
    Watched,
    All,

    Max,
    Default = All,
};

enum class TARuleGhostTagContent {
    Name,
    Time,
    TimeNoLeading,
    Date,

    Max,
    Default = Name,
};

enum class TARuleSolidGhosts {
    None,
    Watched,
    All,

    Max,
    Default = None,
};

enum class TARuleGhostSound {
    None,
    Watched,
    All,

    Max,
    Default = Watched,
};

enum class PageTransitions {
    Disable,
    Enable,

    Max,
    Default = Enable,
};

typedef Settings::Group<Category> Group;
typedef Settings::Entry<Category> Entry;
template <Setting S>
using Helper = Settings::Helper<Setting, S>;

extern const char name[];
extern const char *categoryNames[];
extern const Entry entries[];
constexpr Group group{ name, categoryNames, static_cast<u32>(Setting::Max), entries };

typedef Settings::Settings<Category, ClientSettings::group> Settings;

} // namespace SP::ClientSettings

namespace SP::Settings {

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::DriftMode> {
    using type = SP::ClientSettings::DriftMode;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::HUDLabels> {
    using type = SP::ClientSettings::HUDLabels;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::FOV169> {
    using type = SP::ClientSettings::FOV169;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::MapIcons> {
    using type = SP::ClientSettings::MapIcons;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::InputDisplay> {
    using type = SP::ClientSettings::InputDisplay;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TARuleClass> {
    using type = SP::ClientSettings::TARuleClass;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TARuleGhostSorting> {
    using type = SP::ClientSettings::TARuleGhostSorting;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TARuleGhostTagVisibility> {
    using type = SP::ClientSettings::TARuleGhostTagVisibility;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TARuleGhostTagContent> {
    using type = SP::ClientSettings::TARuleGhostTagContent;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TARuleSolidGhosts> {
    using type = SP::ClientSettings::TARuleSolidGhosts;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TARuleGhostSound> {
    using type = SP::ClientSettings::TARuleGhostSound;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::MiiAvatar> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::MiiClient> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::LoadingScreenColor> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::PageTransitions> {
    using type = SP::ClientSettings::PageTransitions;
};

}
