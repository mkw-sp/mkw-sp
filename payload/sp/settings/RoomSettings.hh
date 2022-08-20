#pragma once

#include "sp/settings/ClientSettings.hh"

namespace SP::RoomSettings {

/*enum class Setting {
    TeamSize,
    TeamSelection,
    RaceCount,
    CourseSelection,
    Class,
    Vehicles,
};*/

//extern const ClientSettings::Entry entries[];
constexpr u32 offset = static_cast<u32>(ClientSettings::Setting::RoomTeamSize);
constexpr u32 count = static_cast<u32>(ClientSettings::Setting::RoomVehicles) - offset + 1;

} // namespace SP::RoomSettings
