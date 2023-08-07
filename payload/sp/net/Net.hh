#pragma once

#include <Common.hh>

#include <memory>

namespace SP::Net {

void *Alloc(s32 size);
void Free(void *block, s32 size);

template <typename T>
class Deleter {
public:
    Deleter(u32 count) : m_count(count) {}

    u32 getCount() const {
        return m_count;
    }

    void operator()(T array[]) {
        Free(array, m_count * sizeof(T));
    }

private:
    u32 m_count;
};

template <typename T>
std::unique_ptr<T[], Deleter<T>> Alloc(u32 count) {
    return std::unique_ptr<T[], Deleter<T>>(reinterpret_cast<T *>(Alloc(count * sizeof(T))), count);
}

template <typename T>
u32 GetCount(const std::unique_ptr<T[], Deleter<T>> &array) {
    return array.get_deleter().getCount();
}

template <typename T>
u32 GetSize(const std::unique_ptr<T[], Deleter<T>> &array) {
    return array.get_deleter().getCount() * sizeof(T);
}

void Init();
void Restart();

} // namespace SP::Net
