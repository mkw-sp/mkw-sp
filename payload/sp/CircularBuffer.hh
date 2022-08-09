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

    bool push(T &&val) {
        if (full()) {
            return false;
        }

        new (&m_vals[(m_front + m_count) % N]) T(val);
        m_count++;
        return true;
    }

    void pop() {
        std::destroy_at(std::launder(reinterpret_cast<T *>(&m_vals[m_front])));
        m_front = (m_front + 1) % N;
        m_count--;
    }

    void reset() {
        while (!empty()) {
            pop();
        }
    }

private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type m_vals[N];
    size_t m_front = 0;
    size_t m_count = 0;
};

} // namespace SP
