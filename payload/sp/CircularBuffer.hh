#pragma once

#include <optional>

#include <Common.hh>

namespace SP {

template <typename T, size_t N>
class CircularBuffer {
public:
    CircularBuffer() = default;

    ~CircularBuffer() {
        reset();
    }

    bool empty() const {
        return m_count == 0;
    }

    bool full() const {
        return m_count == N;
    }

    T *front() {
        if (empty()) {
            return nullptr;
        }

        return std::launder(reinterpret_cast<T *>(&m_vals[m_front]));
    }

    T *back() {
        if (empty()) {
            return nullptr;
        }

        return std::launder(reinterpret_cast<T *>(&m_vals[(m_front + m_count - 1) % N]));
    }

    bool push_back(const T &&val) {
        if (full()) {
            return false;
        }

        new (&m_vals[(m_front + m_count) % N]) T(val);
        m_count++;
        return true;
    }

    void pop_front() {
        std::destroy_at(std::launder(reinterpret_cast<T *>(&m_vals[m_front])));
        m_front = (m_front + 1) % N;
        m_count--;
    }

    size_t count() const {
        return m_count;
    }

    const T *operator[](size_t index) const {
        return std::launder(reinterpret_cast<const T *>(&m_vals[(m_front + index) % N]));
    }

    T *operator[](size_t index) {
        return std::launder(reinterpret_cast<T *>(&m_vals[(m_front + index) % N]));
    }

    bool contains(const T &needle) {
        for (u32 i = 0; i < m_count; i += 1) {
            if (*(*this)[i] == needle) {
                return true;
            }
        }

        return false;
    }

    void remove(size_t index) {
        std::destroy_at(std::launder(reinterpret_cast<T *>(&m_vals[(m_front + index) % N])));
        for (; index < m_count - 1; index++) {
            size_t i0 = (m_front + index) % N;
            size_t i1 = (m_front + index + 1) % N;
            new (&m_vals[i0]) T(std::move(*std::launder(reinterpret_cast<T *>(&m_vals[i1]))));
        }
        m_count--;
    }

    void reset() {
        while (!empty()) {
            pop_front();
        }
    }

private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type m_vals[N];
    size_t m_front = 0;
    size_t m_count = 0;
};

} // namespace SP
