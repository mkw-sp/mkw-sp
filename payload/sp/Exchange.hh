#pragma once

#include "sp/ScopeLock.hh"

extern "C" {
#include <revolution.h>
}

#include <optional>

namespace SP {

struct Empty {
} final;

template <typename T, typename U>
class Exchange {
public:
    U left(T left) {
        ScopeLock<Mutex> mutexLock(m_leftMutex);
        ScopeLock<NoInterrupts> noInterruptsLock;
        m_left.emplace(left);
        if (m_right) {
            U right = std::move(*m_right);
            m_right.reset();
            OSWakeupThread(&m_rightQueue);
            if (m_left) {
                OSSleepThread(&m_leftQueue);
            }
            return right;
        }
        OSSleepThread(&m_leftQueue);
        U right = std::move(*m_right);
        m_right.reset();
        OSWakeupThread(&m_rightQueue);
        return right;
    }

    T right(U right) {
        ScopeLock<Mutex> mutexLock(m_rightMutex);
        ScopeLock<NoInterrupts> noInterruptsLock;
        m_right.emplace(right);
        if (m_left) {
            T left = std::move(*m_left);
            m_left.reset();
            OSWakeupThread(&m_leftQueue);
            if (m_right) {
                OSSleepThread(&m_rightQueue);
            }
            return left;
        }
        OSSleepThread(&m_rightQueue);
        T left = std::move(*m_left);
        m_left.reset();
        OSWakeupThread(&m_leftQueue);
        return left;
    }

private:
    Mutex m_leftMutex;
    Mutex m_rightMutex;
    OSThreadQueue m_leftQueue;
    OSThreadQueue m_rightQueue;
    std::optional<T> m_left;
    std::optional<U> m_right;
};

} // namespace SP
