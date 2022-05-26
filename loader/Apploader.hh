#pragma once

#include "DI.hh"

#include <optional>

namespace Apploader {

typedef void (*GameEntryFunc)(void);

std::optional<GameEntryFunc> LoadAndRun(IOS::DI &di);

} // namespace Apploader
