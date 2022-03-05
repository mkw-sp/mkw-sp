#include "LogFile.h"

#include "ScopeLock.h"
#include "Storage.h"

#include <stdio.h>
#include <string.h>

static bool isInit = false;
static OSMutex fileMutex;
static File file;
static OSTime startTime;
static OSMutex listMutex;
static ThreadNode *listHead = NULL;

void LogFile_init(void) {
    OSInitMutex(&fileMutex);
    if (!Storage_open(&file, L"/mkw-sp/log.txt", MODE_WRITE | MODE_CREATE_ALWAYS)) {
        return;
    }

    startTime = OSGetTime();

    OSInitMutex(&listMutex);

    isInit = true;
}

void LogFile_vprintf(const char *restrict format, va_list args) {
    if (!isInit) {
        return;
    }

    {
        OSThread *thread = OSGetCurrentThread();
        SP_SCOPED_MUTEX_LOCK(listMutex);
        ThreadNode *node = listHead;
        while (node) {
            if (node->thread == thread) {
                return;
            }
            node = node->next;
        }
    }

    OSTime currentTime = OSGetTime();
    u32 secs = OSTicksToSeconds(currentTime - startTime);
    u32 msecs = OSTicksToMilliseconds(currentTime - startTime) % 1000;

    char buffer[128];
    u32 length = snprintf(buffer, sizeof(buffer), "[%lu.%03lu] ", secs, msecs);
    vsnprintf(buffer + length, sizeof(buffer) - length, format, args);

    {
        SP_SCOPED_MUTEX_LOCK(fileMutex);
        u32 offset = Storage_size(&file);
        Storage_write(&file, buffer, strlen(buffer), offset);
        Storage_sync(&file);
    }
}

void LogFile_disable(LogFileDisableGuard *guard) {
    SP_SCOPED_MUTEX_LOCK(listMutex);
    guard->node = (ThreadNode) {
        .thread = OSGetCurrentThread(),
        .next = listHead,
    };
    listHead = &guard->node;
}

void LogFile_restore(LogFileDisableGuard *guard) {
    SP_SCOPED_MUTEX_LOCK(listMutex);
    ThreadNode **next = &listHead;
    while (*next) {
        if (*next == &guard->node) {
            *next = (*next)->next;
            break;
        }
        next = &(*next)->next;
    }
}
