#pragma once

#include <optional>

#include <Common.hh>

namespace LZMA {

std::optional<size_t> Decode(const u8 *src, u8 *dst, size_t srcSize, size_t dstSize);

} // namespace LZMA
