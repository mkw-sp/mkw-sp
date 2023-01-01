#pragma once

#include "sp/settings/ClientSettings.hh"

namespace SP::RoomSettings {

constexpr u32 offset = static_cast<u32>(ClientSettings::Setting::RoomTeamSize);
constexpr u32 count = static_cast<u32>(ClientSettings::Setting::RoomVehicles) - offset + 1;

} // namespace SP::RoomSettings
