#pragma once

#include "DI.hh"

namespace Apploader {

typedef void (*GameEntryFunc)(void);

std::optional<GameEntryFunc> LoadAndRun(IOS::DI &di);

} // namespace Apploader
