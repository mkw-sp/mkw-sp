#pragma once

#include <compare>

extern "C" {
#include "Common.h"
}

#include <array>
#include <optional>
#include <type_traits>
#include <utility>

// This works on Clang, too. Not limited to function arguments.
#define ALIGNED_STRING(s) \
    []() { \
        alignas(32) static const char t[] = s; \
        return t; \
    }()

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
// https://godbolt.org/z/vhxdsbdqG
//
// In particular:
// - Move-only types work
// - Copy-only types work
// - Result<void> types work
//
// Trick to avoid copies via an rvalue-valued rvalue-member function inspired from from SerenityOS https://github.com/SerenityOS/serenity/blob/master/AK/Try.h
// (Thanks to @InusualZ for pointing this out)
//
// (The `MyMove` function is some glue I came up with for the `void` case; perhaps there is a more elegant way?)
//
#if (defined(__clang__) || defined(__GNUC__) || defined(__APPLE__)) && defined(__cpp_lib_remove_cvref) && __cpp_lib_remove_cvref >= 201711L
#define HAS_RUST_TRY
template <typename T> auto MyMove(T&& t) {
  if constexpr (!std::is_void_v<typename std::remove_cvref_t<T>::value_type>) {
    return std::move(*t);
  }
}
#define TRY(...)                                                               \
  ({                                                                           \
    auto&& y = (__VA_ARGS__);                                                  \
    static_assert(!std::is_lvalue_reference_v<decltype(MyMove(y))>);           \
    if (!y) [[unlikely]] {                                                     \
      return std::unexpected(y.error());                                       \
    }                                                                          \
    MyMove(y);                                                                 \
  })
#else
#define TRY(...) static_assert(false, "Compiler does not support TRY macro")
#endif

template <typename T>
void RequireOpt(std::optional<T>) {}

#define TRY_OPT(opt) ({ \
    auto y = (opt); \
    RequireOpt(y); \
    if (!y.has_value()) [[unlikely]] { \
        return std::nullopt; \
    } \
    *y; \
})
// clang-format on
