#include "ScopeLock.hh"

namespace SP {

Mutex::Mutex() {
    OSInitMutex(this);
}

Mutex::~Mutex() = default;

void Mutex::lock() {
    OSLockMutex(this);
}

void Mutex::unlock() {
    OSUnlockMutex(this);
}

ScopeLock<Mutex>::ScopeLock(Mutex &mutex) : m_mutex(mutex) {
    m_mutex.lock();
}

ScopeLock<Mutex>::~ScopeLock() {
    m_mutex.unlock();
}

ScopeLock<NoInterrupts>::ScopeLock() {
    m_isr = OSDisableInterrupts();
}

ScopeLock<NoInterrupts>::~ScopeLock() {
    OSRestoreInterrupts(m_isr);
}

} // namespace SP
