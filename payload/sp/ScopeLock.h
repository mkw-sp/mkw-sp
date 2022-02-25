#pragma once

#include <revolution.h>

typedef struct {
    OSMutex *mutex;
} SpMutexScopeLock;
static inline void SpMutexScopeLock_destroy(SpMutexScopeLock *lock) {
    OSUnlockMutex(lock->mutex);
}

#define SP_SCOPED_MUTEX_LOCK(m)                                                \
    OSLockMutex(&m);                                                           \
    SpMutexScopeLock _lock __attribute__((cleanup(SpMutexScopeLock_destroy))); \
    _lock.mutex = &m
