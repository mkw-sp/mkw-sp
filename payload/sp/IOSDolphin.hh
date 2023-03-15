#pragma once

extern "C" {
#include <revolution/ios.h>
}
#include "FixedString.hh"
#include <Common.h>
#include <array>
#include <expected>
#include <string_view>

namespace SP::IOSDolphin {

template <typename T>
using Result = std::expected<T, IPCResult>;

bool Open();
bool IsOpen();
// Not thread-safe: We can't assume once a call to Open() is made, a context switch won't
// Close() it.
void Close();

// Result: Milliseconds
Result<u32> GetElapsedTime();
Result<std::array<char, 64>> GetVersion();
// Result: Ticks per second
Result<u32> GetCPUSpeed();
// Result: Percent [0-200]
Result<u32> GetSpeedLimit();
IPCResult SetSpeedLimit(u32 percent);
// The code is 3 characters long (dolphin default: 'DOL')
Result<std::array<char, 3>> GetRealProductCode();

//! Calls down to Discord_Initialize (passing applicationId); Dolphin will call
//! Discord_Shutdown before if it was already initialized.
//!
//! Will return IPC_EACCES if Config::MAIN_USE_DISCORD_PRESENCE is not set.
IPCResult DiscordSetClient(std::string_view client);

struct DolphinDiscordPresence {
    FixedString<128> state;
    FixedString<128> details;
    s64 startTimestamp = {};
    s64 endTimestamp = {};
    FixedString<32> largeImageKey;
    FixedString<128> largeImageText;
    FixedString<32> smallImageKey;
    FixedString<128> smallImageText;
    s32 partySize = {};
    s32 partyMax = {};

    // Missing fields from DiscordRichPresence in discord_rpc.h:
    //
    //    const char* partyId;        /* max 128 bytes */
    //    int partyPrivacy;
    //    const char* matchSecret;    /* max 128 bytes */
    //    const char* joinSecret;     /* max 128 bytes */
    //    const char* spectateSecret; /* max 128 bytes */
    //    int8_t instance;
    //
    // https://github.com/discord/discord-rpc/blob/master/include/discord_rpc.h#L26

    constexpr bool operator==(const DolphinDiscordPresence &) const = default;
};

//! Calls Discord_UpdatePresence
IPCResult DiscordSetPresence(const DolphinDiscordPresence &presence);

//! NOTE: As of 4c2d707/5.0-17155, `DiscordReset` is just a call to
//! `IOSDolphin::DiscordSetClient` with an empty string.
IPCResult DiscordReset();

// Result: Unix timestamp (milliseconds)
Result<u64> GetSystemTime();

} // namespace SP::IOSDolphin
