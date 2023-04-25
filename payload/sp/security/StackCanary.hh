#pragma once

#include <Common.h>

namespace SP::StackCanary {

void AddLinkRegisterPatches(u32 *startAddress, u32 *endAddress);

} // namespace SP::StackCanary
