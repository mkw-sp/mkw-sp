extern "C" {
#include "LogFile.h"
}

#include "sp/ScopeLock.hh"
#include "sp/storage/Storage.hh"

#include <cstdio>

namespace SP::LogFile {

static const size_t BUFFER_SIZE = 4096;
static bool isInit = false;
static OSTime startTime;
static char buffers[BUFFER_SIZE][2];
static u8 index = 0;
static u16 offset = 0;
static OSThread thread;
static u8 stack[4096];

static void *Run(void *UNUSED(arg)) {
    auto file = Storage::Open(L"/mkw-sp/log.txt", "w");
    if (!file) {
        return nullptr;
    }

    while (true) {
        OSSleepMilliseconds(50);

        u8 oldIndex;
        u16 oldOffset;
        {
            ScopeLock<NoInterrupts> lock;

            if (offset == 0) {
                continue;
            }

            oldIndex = index;
            index ^= 1;

            oldOffset = offset;
            offset = 0;
        }

        file->write(buffers[oldIndex], oldOffset, file->size());
        file->sync();
    }
}

static void Init() {
    startTime = OSGetTime();

    OSCreateThread(&thread, Run, nullptr, stack + sizeof(stack), sizeof(stack), 31, 0);
    OSResumeThread(&thread);

    isInit = true;
}

static void VPrintf(const char *format, va_list args) {
    if (!isInit) {
        return;
    }

    ScopeLock<NoInterrupts> lock;

    u16 maxLength = BUFFER_SIZE - offset;
    OSTime currentTime = OSGetTime();
    u32 secs = OSTicksToSeconds(currentTime - startTime);
    u32 msecs = OSTicksToMilliseconds(currentTime - startTime) % 1000;
    u32 length = snprintf(buffers[index] + offset, maxLength, "[%lu.%03lu] ", secs, msecs);
    if (length >= maxLength) {
        offset = BUFFER_SIZE;
        return;
    }
    offset += length;
    maxLength -= length;
    length = vsnprintf(buffers[index] + offset, maxLength, format, args);
    if (length >= maxLength) {
        offset = BUFFER_SIZE;
        return;
    }
    offset += length;
}

} // namespace SP::LogFile

extern "C" {
void LogFile_Init(void) {
    SP::LogFile::Init();
}

void LogFile_VPrintf(const char *format, va_list args) {
    SP::LogFile::VPrintf(format, args);
}
}
