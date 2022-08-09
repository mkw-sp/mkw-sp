#pragma once

extern "C" {
#include <revolution/ios.h>
}
#include <array>
#include <expected>

namespace SP::IOSDolphin {

template <typename T>
using Result = std::expected<T, IPCResult>;

bool Open();
bool IsOpen();
void Close();

// Result: Milliseconds
Result<u32> GetSystemTime();
Result<std::array<char, 64>> GetVersion();
// Result: Ticks per second
Result<u32> GetCPUSpeed();
// Result: Percent [0-200]
Result<u32> GetSpeedLimit();
IPCResult SetSpeedLimit(u32 percent);
// The code is 3 characters long (dolphin default: 'DOL')
Result<std::array<char, 3>> GetRealProductCode();

}  // namespace SP::IOSDolphin
