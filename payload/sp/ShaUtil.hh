#pragma once

#include <Common.hh>

using Sha1 = std::array<u8, 0x14>;
std::array<char, 40 + 1> sha1ToHex(const Sha1 &sha1);
