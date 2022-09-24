#pragma once

#include <compare>

extern "C" {
#include "Common.h"
}

template <typename T>
T AlignDown(T val, size_t alignment) {
    return val / alignment * alignment;
}

template <typename T>
T AlignUp(T val, size_t alignment) {
    return AlignDown<T>(val + alignment - 1, alignment);
}

template <typename T>
uintptr_t VirtualToPhysical(T *ptr) {
    return reinterpret_cast<uintptr_t>(ptr) & 0x7fffffff;
}

template <typename T>
T *PhysicalToVirtual(uintptr_t addr) {
    return reinterpret_cast<T *>(addr | 0x80000000);
}

static inline std::strong_ordering operator<=>(const VersionInfo& lhs, const VersionInfo &rhs) {
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
