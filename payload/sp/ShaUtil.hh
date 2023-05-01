#pragma once

#include <Common.hh>

#include <expected>
#include <string_view>

using Sha1 = std::array<u8, 0x14>;
std::array<char, 40 + 1> sha1ToHex(const Sha1 &sha1);
std::expected<Sha1, const char *> sha1FromHex(std::string_view sv);
