#pragma once

#include <Common.hh>

namespace DCache {

void Invalidate(void *start, size_t size);

template <typename T>
void Invalidate(T *val) {
    Invalidate(val, sizeof(T));
}

void Flush(const void *start, size_t size);

template <typename T>
void Flush(const T *val) {
    Flush(val, sizeof(T));
}

} // namespace DCache
