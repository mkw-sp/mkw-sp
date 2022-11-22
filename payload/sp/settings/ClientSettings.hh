#pragma once

#include "sp/settings/Settings.hh"

namespace SP::ClientSettings {

enum class Setting {
    // Race
    DriftMode,
    VanillaMode,
    SimplifiedControls,
    FOV169,
    RegionLineColor,
    FarPlayerTags,
    PlayerTags,
    HUDLabels,
    HUDTeamColors,
    MapIcons,
    InputDisplay,
    Speedometer,
    RankControl,

    // Sound
    ItemMusic,
    LastLapJingle,
    LastLapSpeedup,

    // TA
    TAClass,
    TAGhostSorting,
    TAGhostTagVisibility,
    TAGhostTagContent,
    TASolidGhosts,
    TAGhostSound,

    // VS
    VSTeamSize,
    VSRaceCount,
    VSCourseSelection,
    VSClass,
    VSVehicles,
    VSMegaClouds,

    // Room
    RoomTeamSize,
    RoomTeamSelection,
    RoomRaceCount,
    RoomCourseSelection,
    RoomClass,
    RoomVehicles,
    RoomCodeHigh,
    RoomCodeLow,

    // License
    MiiAvatar,
    MiiClient,
    ColorPalette,
    LoadingScreenColor,
    GCPadRumble,
    PageTransitions,
    PerfOverlay,
    RegionFlagDisplay,

    // DebugOverlay
    DebugCheckpoints,
};

enum class Category {
    Race,
    Sound,
    TA,
    VS,
    Room,
    License,
    DebugOverlay,
};

enum class DriftMode {
    Manual,
    Auto,
};

enum class VanillaMode {
    Disable,
    Enable,
};

enum class SimplifiedControls {
    Off,
    NonExclusive,
    Exclusive,
};

enum class FOV169 {
    FOV169,
    FOV43,
};

enum class RegionLineColor {
    Red,
    Blue,
    Green,
    Yellow,
    White,
    Pink,
};

enum class FarPlayerTags {
    Disable,
    Enable,
};

enum class HUDLabels {
    Hide,
    Show,
};

enum class HUDTeamColors {
    Disable,
    Enable,
};

enum class MapIcons {
    Characters,
    Miis,
};

enum class InputDisplay {
    Disable,
    Simple,
};

enum class Speedometer {
    InternalPlus,
    Internal,
    XYZ,
    XZ,
    Y,
};

enum class RankControl {
    GPVS,
    Always,
};

enum class ItemMusic {
    None,
    DamageOnly,
    All,
};

enum class LastLapJingle {
    GameplayOnly,
    Always,
};

enum class LastLapSpeedup {
    None,
    Static,
    Dynamic,
};

enum class TAClass {
    CC150,
    CC200,
};

enum class TAGhostSorting {
    Time,
    Date,
    Flap,
    Lap2Pace,
    Lap3Pace,
};

enum class TAGhostTagVisibility {
    None,
    Watched,
    All,
};

enum class TAGhostTagContent {
    Name,
    Time,
    TimeNoLeading,
    Date,
};

enum class TASolidGhosts {
    None,
    Watched,
    All,
};

enum class TAGhostSound {
    None,
    Watched,
    All,
};

enum class VSTeamSize {
    FFA,
    Two,
    Three,
    Four,
    Six,
};

enum class VSCourseSelection {
    Choose,
    Random,
    InOrder,
};

enum class VSClass {
    Mixed,
    CC150,
    Mirror,
    CC200,
};

enum class VSVehicles {
    All,
    Karts,
    Bikes,
    InsideDrift,
    OutsideDrift,
    Optimal,
    Random,
};

enum class RoomTeamSize {
    FFA,
    Two,
    Three,
    Four,
    Six,
};

enum class RoomTeamSelection {
    Random,
    Host,
    Players,
};

enum class RoomCourseSelection {
    Random,
    InOrder,
    Host,
    Vote,
};

enum class RoomClass {
    Mixed,
    CC150,
    Mirror,
    CC200,
};

enum class RoomVehicles {
    All,
    Karts,
    Bikes,
    InsideDrift,
    OutsideDrift,
    Optimal,
    Random,
};

enum class VSMegaClouds {
    Disable,
    Enable,
};

enum class ColorPalette {
    Vivid,
    Colorblind,
};

enum class GCPadRumble {
    Disable,
    Enable,
};

enum class PageTransitions {
    Disable,
    Enable,
};

enum class PerfOverlay {
    Disable,
    Enable,
};

enum class RegionFlagDisplay{
    Disable,
    Enable,
};

enum class DebugCheckpoints {
    Disable,
    Enable,
};

typedef Settings::Group<Category> Group;
typedef Settings::Entry<Category> Entry;
template <Setting S>
using Helper = Settings::Helper<Setting, S>;

extern const char name[];
constexpr auto categoryNames = magic_enum::enum_names<Category>();
extern const u32 categoryMessageIds[];
constexpr u32 entryCount = magic_enum::enum_count<Setting>();
extern const Entry entries[];
constexpr Group group{ name, categoryNames.data(), categoryMessageIds, entryCount, entries };

typedef Settings::Settings<Category, ClientSettings::group> Settings;

} // namespace SP::ClientSettings

namespace SP::Settings {

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::DriftMode> {
    using type = SP::ClientSettings::DriftMode;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::VanillaMode> {
    using type = SP::ClientSettings::VanillaMode;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::SimplifiedControls> {
    using type = SP::ClientSettings::SimplifiedControls;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::FOV169> {
    using type = SP::ClientSettings::FOV169;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RegionLineColor> {
    using type = SP::ClientSettings::RegionLineColor;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::FarPlayerTags> {
    using type = SP::ClientSettings::FarPlayerTags;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::PlayerTags> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::HUDLabels> {
    using type = SP::ClientSettings::HUDLabels;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::HUDTeamColors> {
    using type = SP::ClientSettings::HUDTeamColors;
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
struct Helper<ClientSettings::Setting, ClientSettings::Setting::Speedometer> {
    using type = SP::ClientSettings::Speedometer;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RankControl> {
    using type = SP::ClientSettings::RankControl;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::ItemMusic> {
    using type = SP::ClientSettings::ItemMusic;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::LastLapJingle> {
    using type = SP::ClientSettings::LastLapJingle;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::LastLapSpeedup> {
    using type = SP::ClientSettings::LastLapSpeedup;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TAClass> {
    using type = SP::ClientSettings::TAClass;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TAGhostSorting> {
    using type = SP::ClientSettings::TAGhostSorting;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TAGhostTagVisibility> {
    using type = SP::ClientSettings::TAGhostTagVisibility;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TAGhostTagContent> {
    using type = SP::ClientSettings::TAGhostTagContent;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TASolidGhosts> {
    using type = SP::ClientSettings::TASolidGhosts;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::TAGhostSound> {
    using type = SP::ClientSettings::TAGhostSound;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::VSTeamSize> {
    using type = SP::ClientSettings::VSTeamSize;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::VSRaceCount> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::VSCourseSelection> {
    using type = SP::ClientSettings::VSCourseSelection;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::VSClass> {
    using type = SP::ClientSettings::VSClass;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::VSVehicles> {
    using type = SP::ClientSettings::VSVehicles;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomTeamSize> {
    using type = SP::ClientSettings::RoomTeamSize;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomTeamSelection> {
    using type = SP::ClientSettings::RoomTeamSelection;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomRaceCount> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomCourseSelection> {
    using type = SP::ClientSettings::RoomCourseSelection;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomClass> {
    using type = SP::ClientSettings::RoomClass;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomVehicles> {
    using type = SP::ClientSettings::RoomVehicles;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::VSMegaClouds> {
    using type = SP::ClientSettings::VSMegaClouds;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomCodeHigh> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RoomCodeLow> {
    using type = u32;
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
struct Helper<ClientSettings::Setting, ClientSettings::Setting::ColorPalette> {
    using type = SP::ClientSettings::ColorPalette;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::LoadingScreenColor> {
    using type = u32;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::GCPadRumble> {
    using type = SP::ClientSettings::GCPadRumble;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::PageTransitions> {
    using type = SP::ClientSettings::PageTransitions;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::PerfOverlay> {
    using type = SP::ClientSettings::PerfOverlay;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::RegionFlagDisplay> {
    using type = SP::ClientSettings::RegionFlagDisplay;
};

template <>
struct Helper<ClientSettings::Setting, ClientSettings::Setting::DebugCheckpoints> {
    using type = SP::ClientSettings::DebugCheckpoints;
};

}
