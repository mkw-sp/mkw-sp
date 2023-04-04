#pragma once

#include <compare>

extern "C" {
#include "Common.h"
}

#include <array>

template <typename T>
T AlignDown(T val, size_t alignment) {
    return val / alignment * alignment;
}

template <typename T>
T AlignUp(T val, size_t alignment) {
    return AlignDown<T>(val + alignment - 1, alignment);
}

static inline std::strong_ordering operator<=>(const VersionInfo &lhs, const VersionInfo &rhs) {
    if (auto cmp = lhs.major <=> rhs.major; cmp != 0) {
        return cmp;
    }
    if (auto cmp = lhs.minor <=> rhs.minor; cmp != 0) {
        return cmp;
    }
    if (auto cmp = lhs.patch <=> rhs.patch; cmp != 0) {
        return cmp;
    }
    return std::strong_ordering::equal;
}

template <typename T>
struct Vec2 {
    T x;
    T y;
};

typedef float Mtx34[3][4];
inline Mtx34 &Decay(std::array<float, 12> &arr) {
    return reinterpret_cast<Mtx34 &>(*arr.data());
}
inline const Mtx34 &Decay(const std::array<float, 12> &arr) {
    return reinterpret_cast<const Mtx34 &>(*arr.data());
}

// clang-format off
//
// ```cpp
//    std::expected<int, Err> GetInt();
//    std::expected<int, Err> Foo() {
//       return TRY(GetInt()) + 5;
//    }
// ```
//
// This unfortunately may eagerly copy when a move is preferable
//
#if defined(__clang__) || defined(__GNUC__) || defined(__APPLE__)
#define HAS_RUST_TRY
#define TRY(...)                                                               \
  ({                                                                           \
    auto y = (__VA_ARGS__);                                                    \
    if (!y) {                                                                  \
      return std::unexpected(y.error());                                       \
    }                                                                          \
    *y;                                                                        \
  })
#else
#define TRY(...) static_assert(false, "Compiler does not support TRY macro")
#endif
// clang-format on
