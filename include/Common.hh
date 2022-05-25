#pragma once

extern "C" {
#include "Common.h"
}

template <typename T>
T AlignUp(T val, size_t alignment) {
    return AlignDown<T>(reinterpret_cast<size_t>(val) + alignment - 1, alignment);
}

template <typename T>
T AlignDown(T val, size_t alignment) {
    return reinterpret_cast<T>(reinterpret_cast<size_t>(val) / alignment * alignment);
}

template <typename T>
uintptr_t VirtualToPhysical(T *ptr) {
    return reinterpret_cast<uintptr_t>(ptr) & 0x7fffffff;
}

template <typename T>
T *PhysicalToVirtual(uintptr_t addr) {
    return reinterpret_cast<T *>(addr | 0x80000000);
}
