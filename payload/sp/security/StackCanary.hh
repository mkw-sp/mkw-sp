#pragma once

#include <Common.hh>

namespace SP::StackCanary {

void AddLinkRegisterPatches(u32 *startAddress, u32 *endAddress);

} // namespace SP::StackCanary
