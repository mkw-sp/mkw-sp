#pragma once

#include <revolution.hh>

namespace SP {

class Mutex final : private OSMutex {
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();
};

class NoInterrupts final {};

template <typename T>
class ScopeLock;

template <>
class ScopeLock<Mutex> {
public:
    ScopeLock(Mutex &mutex);
    ~ScopeLock();

private:
    Mutex &m_mutex;
};

template <>
class ScopeLock<NoInterrupts> {
public:
    ScopeLock();
    ~ScopeLock();

private:
    u32 m_isr;
};

} // namespace SP
