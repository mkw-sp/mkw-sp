#pragma once

#include <Common.hh>

#include <expected>
#include <span>
#include <string_view>

#include "sp/FixedString.hh"

namespace SP {

[[nodiscard]] FixedString<0x28 + 1> SHA1ToHex(std::span<const u8, 0x14> bytes);
[[nodiscard]] std::expected<std::array<u8, 0x14>, const char *> SHA1FromHex(std::string_view view);

} // namespace SP
