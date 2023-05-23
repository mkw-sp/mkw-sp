#pragma once

extern "C" {
#include "Rel.h"
}

#include <expected>

namespace SP::Rel {

std::expected<void, const char *> Load();

} // namespace SP::Rel
