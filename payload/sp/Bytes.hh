#pragma once

#include <bit>

namespace SP::Bytes {

template <typename T, std::endian E = std::endian::big>
T Read(const u8 *src, size_t offset) {
    T val = 0;
    #pragma GCC unroll(8)
    for (size_t i = 0; i < sizeof(T); i++) {
        switch (E) {
        case std::endian::big:
            val |= static_cast<T>(src[offset + i]) << (8 * (sizeof(T) - i - 1));
            break;
        case std::endian::little:
            val |= static_cast<T>(src[offset + i]) << (8 * i);
            break;
        }
    }
    return val;
}

template <typename T, std::endian E = std::endian::big>
void Write(u8 *dst, size_t offset, T val) {
    #pragma GCC unroll(8)
    for (size_t i = 0; i < sizeof(T); i++) {
        switch (E) {
        case std::endian::big:
            dst[offset + i] = val >> (8 * (sizeof(T) - i - 1));
            break;
        case std::endian::little:
            dst[offset + i] = val >> (8 * i);
            break;
        }
    }
}

} // namespace SP::Bytes
