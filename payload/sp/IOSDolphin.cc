#include "IOSDolphin.hh"
#include <span>
#include <vendor/magic_enum/magic_enum.hpp>

extern "C" {
#include <revolution.h>
#include <stdalign.h>
}

namespace SP::IOSDolphin {

// Debug logging
#define IOSDOLPHIN_LOG SP_LOG

enum class Ioctlv {
    // Merged in df32e3f Nov 10, 2019
    //
    // Dolphin 5.0-11186
    GetElapsedTime = 1, // Vector IN() OUT(u32)
    GetVersion = 2, // Vector IN() OUT(char[])
    GetSpeedLimit = 3, // Vector IN() OUT(u32)
    SetSpeedLimit = 4, // Vector IN(u32) OUT()
    GetCPUSpeed = 5, // Vector IN() OUT(u32)

    // Merged in 393ce52 May 22, 2020
    //
    // Dolphin 5.0-12058
    GetRealProductCode = 6, // Vector IN() OUT(char[])

    // Merged in 4c2d707 August 7, 2022
    //
    // Dolphin 5.0-17155
    //
    // NOTE: As of 4c2d707/5.0-17155, `DiscordReset` is just a call to `DiscordSetClient`
    // with an empty string.
    DiscordSetClient = 7, // Vector IN(char[]) OUT()
    DiscordSetPresence = 8, // Vector IN(char[], char[], char[], char[], char[], s64,
                            // s64, u32, u32) OUT()
    DiscordReset = 9, // Vector IN() OUT()

    // Merged in 083b817 November 12, 2022
    //
    // Dolphin 5.0-17856
    GetSystemTime = 10, // Vector IN() OUT(u64)
};

static s32 sDevDolphin = -1;

bool Open() {
    if (sDevDolphin < 0) {
        sDevDolphin = IOS_Open("/dev/dolphin", 0);
    }
    return sDevDolphin >= 0;
}

bool IsOpen() {
    return sDevDolphin >= 0;
}

void Close() {
    if (sDevDolphin >= 0) {
        IOS_Close(sDevDolphin);
        sDevDolphin = -1;
    }
}

namespace {

IPCResult IssueCommand(Ioctlv cmd, u32 inputs, u32 outputs, std::span<IOVector> vec) {
    assert(inputs + outputs == vec.size());
    const IPCResult result = static_cast<IPCResult>(
            IOS_Ioctlv(sDevDolphin, static_cast<IOSCommand>(cmd), inputs, outputs, vec.data()));
    if (result == IPC_OK) {
        return IPC_OK;
    }
    const std::string_view cmd_string = magic_enum::enum_name(cmd);
    const std::string_view err_string = magic_enum::enum_name(result);
    IOSDOLPHIN_LOG("%.*s: Failed with IPC error code %i (%.*s)", cmd_string.length(),
            cmd_string.data(), static_cast<int>(result), err_string.length(), err_string.data());
    IOSDOLPHIN_LOG("-> IOS_Ioctlv(%i, %x, %i, %i, %p)", sDevDolphin, static_cast<IOSCommand>(cmd),
            inputs, outputs, vec.data());
    for (size_t i = 0; i < vec.size(); ++i) {
        const auto &it = vec[i];
        IOSDOLPHIN_LOG(" [%u] @%p: IOVector { .data = %p, .size = 0x%x }", static_cast<unsigned>(i),
                &it, it.data, it.size);
    }
    return result;
}

IPCResult IssueCommand(Ioctlv cmd, u32 inputs, u32 outputs, IOVector &vec) {
    return IssueCommand(cmd, inputs, outputs, std::span<IOVector>(&vec, 1));
}

} // namespace

// Result: Milliseconds
Result<u32> GetElapsedTime() {
    assert(IsOpen());

    u32 ms = 0;
    alignas(32) IOVector vec = {.data = &ms, .size = sizeof(ms)};
    const IPCResult result = IssueCommand(Ioctlv::GetElapsedTime, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }

    return ms;
}

// Result: "5.0-XXXXX"
Result<std::array<char, 64>> GetVersion() {
    assert(IsOpen());
    std::array<char, 64> query = {};

    std::span<char> query_span = query;
    alignas(32) IOVector vec = {
            .data = query_span.data(),
            .size = query_span.size_bytes(),
    };
    const IPCResult result = IssueCommand(Ioctlv::GetVersion, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }
    if (query[query.size() - 1] != '\0') {
        query[query.size() - 1] = '\0';
        // This should never happen.
        IOSDOLPHIN_LOG("GetVersion: Dolphin returned invalid version string: %s\n", query.data());
        return std::unexpected(IPC_EINVAL);
    }
    return query;
}

// Result: Ticks per second
Result<u32> GetCPUSpeed() {
    assert(IsOpen());
    u32 query = {};
    alignas(32) IOVector vec = {.data = &query, .size = sizeof(query)};
    const IPCResult result = IssueCommand(Ioctlv::GetCPUSpeed, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }
    return query;
}

// Result: Percent [0-200]
Result<u32> GetSpeedLimit() {
    assert(IsOpen());
    u32 query = {};
    alignas(32) IOVector vec = {.data = &query, .size = sizeof(query)};
    const IPCResult result = IssueCommand(Ioctlv::GetSpeedLimit, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }
    return query;
}

IPCResult SetSpeedLimit(u32 percent) {
    assert(IsOpen());
    alignas(32) IOVector vec = {.data = &percent, .size = sizeof(percent)};
    return IssueCommand(Ioctlv::SetSpeedLimit, 1, 0, vec);
}

// The code is 3 characters long (dolphin default: 'DOL')
Result<std::array<char, 3>> GetRealProductCode() {
    assert(IsOpen());
    std::array<char, 4> query = {};

    std::span<char> query_span = query;
    alignas(32) IOVector vec = {
            .data = query_span.data(),
            .size = query_span.size_bytes(),
    };
    const IPCResult result = IssueCommand(Ioctlv::GetRealProductCode, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }
    if (query[query.size() - 1] != '\0') {
        query[query.size() - 1] = '\0';
        // This should never happen.
        IOSDOLPHIN_LOG("GetRealProductCode: Dolphin returned invalid code string: %s\n",
                query.data());
        return std::unexpected(IPC_EINVAL);
    }
    const std::array<char, 3> arr = {query[0], query[1], query[2]};
    return arr;
}

IPCResult DiscordSetClient(std::string_view client) {
    IOSDOLPHIN_LOG("DiscordSetClient: Setting client to %.*s", client.length(), client.data());
    assert(IsOpen());
    alignas(32) IOVector vec = {
            .data = const_cast<char *>(client.data()),
            .size = client.size(),
    };
    return IssueCommand(Ioctlv::DiscordSetClient, 1, 0, vec);
}

IPCResult DiscordSetPresence(const DolphinDiscordPresence &presence) {
    IOSDOLPHIN_LOG("DiscordSetPresence: Setting presence");
    assert(IsOpen());
    alignas(32) std::array<IOVector, 10> vec = {};

    const auto set = [&](size_t index, const auto span) {
        vec[index] = IOVector{
                .data = const_cast<decltype(span)::value_type *>(span.data()),
                .size = span.size() * sizeof(span[0]),
        };
    };
    const auto set_raw = [&](size_t index, const auto &thing) {
        vec[index] = IOVector{
                .data = const_cast<void *>(reinterpret_cast<const void *>(&thing)),
                .size = sizeof(thing),
        };
    };

    set(0, std::string_view(presence.details));
    set(1, std::string_view(presence.state));
    set(2, std::string_view(presence.largeImageKey));
    set(3, std::string_view(presence.largeImageText));
    set(4, std::string_view(presence.smallImageKey));
    set(5, std::string_view(presence.smallImageText));
    set_raw(6, presence.startTimestamp);
    set_raw(7, presence.endTimestamp);
    set_raw(8, presence.partySize);
    set_raw(9, presence.partyMax);

    return IssueCommand(Ioctlv::DiscordSetPresence, vec.size(), 0, vec);
}

IPCResult DiscordReset() {
    IOSDOLPHIN_LOG("DiscordReset: Resetting client");
    assert(IsOpen());
    return IssueCommand(Ioctlv::DiscordReset, 0, 0, {});
}

// Result: Milliseconds
Result<u64> GetSystemTime() {
    assert(IsOpen());

    u64 ms = 0;
    alignas(32) IOVector vec = {.data = &ms, .size = sizeof(ms)};
    const IPCResult result = IssueCommand(Ioctlv::GetSystemTime, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }

    return ms;
}

} // namespace SP::IOSDolphin
