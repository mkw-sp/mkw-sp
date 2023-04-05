#pragma once

#include <Common.hh>

#include <optional>

namespace Dol {

const void *GetStartAddress();

std::optional<u32> GetSize();

std::optional<bool> IsClean();

} // namespace Dol
