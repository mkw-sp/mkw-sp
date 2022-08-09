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
    GetSystemTime = 1,  // Vector IN() OUT(u32)
    GetVersion = 2,     // Vector IN() OUT(char[])
    GetSpeedLimit = 3,  // Vector IN() OUT(u32)
    SetSpeedLimit = 4,  // Vector IN(u32) OUT()
    GetCPUSpeed = 5,    // Vector IN() OUT(u32)

    // Merged in 393ce52 May 22, 2020
    //
    // Dolphin 5.0-12058
    GetRealProductCode = 6,  // Vector IN() OUT(char[])
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
IPCResult IssueCommand(Ioctlv cmd, u32 inputs, u32 outputs, IOVector &vec) {
    const IPCResult result = static_cast<IPCResult>(
            IOS_Ioctlv(sDevDolphin, static_cast<IOSCommand>(cmd), inputs, outputs, &vec));
    if (result == IPC_OK) {
        return IPC_OK;
    }
    const std::string_view cmd_string = magic_enum::enum_name(cmd);
    const std::string_view err_string = magic_enum::enum_name(result);
    IOSDOLPHIN_LOG("%.*s: Failed with IPC error code %i (%.*s)\n", cmd_string.length(),
            cmd_string.data(), static_cast<int>(result), err_string.length(),
            err_string.data());
    return result;
}
}  // namespace

// Result: Milliseconds
Result<u32> GetSystemTime() {
    assert(IsOpen());

    u32 ms = 0;
    alignas(32) IOVector vec = { .data = &ms, .size = sizeof(ms) };
    const IPCResult result = IssueCommand(Ioctlv::GetSystemTime, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }

    return ms;
}
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
        IOSDOLPHIN_LOG("GetVersion: Dolphin returned invalid version string: %s\n",
                query.data());
        return std::unexpected(IPC_EINVAL);
    }
    return query;
}
// Result: Ticks per second
Result<u32> GetCPUSpeed() {
    assert(IsOpen());
    u32 query = {};
    alignas(32) IOVector vec = { .data = &query, .size = sizeof(query) };
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
    alignas(32) IOVector vec = { .data = &query, .size = sizeof(query) };
    const IPCResult result = IssueCommand(Ioctlv::GetSpeedLimit, 0, 1, vec);
    if (result != IPC_OK) {
        return std::unexpected(result);
    }
    return query;
}
IPCResult SetSpeedLimit(u32 percent) {
    assert(IsOpen());
    alignas(32) IOVector vec = { .data = &percent, .size = sizeof(percent) };
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
        IOSDOLPHIN_LOG("GetVersion: Dolphin returned invalid version string: %s\n",
                query.data());
        return std::unexpected(IPC_EINVAL);
    }
    const std::array<char, 3> arr = { query[0], query[1], query[2] };
    return arr;
}

}  // namespace SP::IOSDolphin
