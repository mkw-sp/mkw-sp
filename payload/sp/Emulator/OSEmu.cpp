//
// Cross-platform implementation of OSMutex and OSReport via C++11 std::mutex,
// std::thread.
//

extern "C" {
#include <revolution.h>
}

#include <stdarg.h>
#include <mutex>
#include <unordered_map>

std::unordered_map<OSMutex *, std::mutex> gMutexMap;
std::mutex gMutexMapLock;

extern "C" void OSInitMutex(OSMutex *mutex) {
    std::lock_guard<std::mutex> g(gMutexMapLock);

    gMutexMap[mutex];
}
extern "C" void OSLockMutex(OSMutex *mutex) {
    std::mutex *m = nullptr;
    {
        std::lock_guard<std::mutex> g(gMutexMapLock);
        m = &gMutexMap.at(mutex);
    }
    assert(m);
    m->lock();
}
extern "C" void OSUnlockMutex(OSMutex *mutex) {
    std::mutex *m = nullptr;
    {
        std::lock_guard<std::mutex> g(gMutexMapLock);
        m = &gMutexMap.at(mutex);
    }
    assert(m);
    m->unlock();
}

extern "C" void OSReport(const char *s, ...) {
    va_list args;
    va_start(args, s);
    vprintf(s, args);
    va_end(args);
}