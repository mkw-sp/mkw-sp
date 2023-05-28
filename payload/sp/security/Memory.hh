#pragma once

#include <Common.hh>

namespace SP::Memory {

void InvalidateAllIBATs();

void ProtectRange(u32 channel, void *start, void *end, u32 permissions);

} // namespace SP::Memory
