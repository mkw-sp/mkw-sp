extern "C" {
#include "LogFile.h"
}

#include "sp/ScopeLock.hh"
#include "sp/settings/GlobalSettings.hh"
#include "sp/storage/Storage.hh"

#include <cstdio>
#include <cwctype>
#include <string_view>

#define LOG_FILE_DIRECTORY L"/mkw-sp/logs"
#define LOG_FILE_EXTENSION L".log"
#define LOG_FILE_EXTENSION_LENGTH (sizeof(LOG_FILE_EXTENSION) / sizeof(wchar_t) - 1)
#define LOG_FILE_NAME_FORMAT L"DDDD-DD-DD-DD-DD-DD"
#define LOG_FILE_NAME_LENGTH (sizeof(LOG_FILE_NAME_FORMAT) / sizeof(wchar_t) - 1)

namespace SP::LogFile {

static const size_t BUFFER_SIZE = 4096;
static bool isInit = false;
static OSTime startTime;
static char buffers[2][BUFFER_SIZE];
static u8 index = 0;
static u16 offset = 0;
static u8 stack[8192];
static OSThread thread;

static void *Run(void * /* arg */) {
    Storage::CreateDir(LOG_FILE_DIRECTORY, true);

    OSCalendarTime time;
    OSTicksToCalendarTime(OSGetTime(), &time);

    wchar_t logFilePath[48];
    swprintf(logFilePath, sizeof(logFilePath),
            LOG_FILE_DIRECTORY L"/%04d-%02d-%02d-%02d-%02d-%02d" LOG_FILE_EXTENSION, time.year,
            time.mon + 1, time.mday, time.hour, time.min, time.sec);

    auto file = Storage::Open(logFilePath, "w");
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

static bool IsValidLogFile(Storage::NodeInfo nodeInfo) {
    if (nodeInfo.type != Storage::NodeType::File) {
        return false;
    }
    if (wcslen(nodeInfo.name) != LOG_FILE_NAME_LENGTH + LOG_FILE_EXTENSION_LENGTH) {
        return false;
    }

    for (size_t n = 0; n < LOG_FILE_NAME_LENGTH; n++) {
        const wchar_t wc = nodeInfo.name[n];

        if (LOG_FILE_NAME_FORMAT[n] == L'D') {
            if (!iswdigit(wc)) {
                return false;
            }
        } else {
            if (wc != L'-') {
                return false;
            }
        }
    }

    return std::wstring_view(nodeInfo.name).ends_with(LOG_FILE_EXTENSION);
}

static bool ShouldDeleteLogFile(OSTime tick) {
    u32 logFileRetention = SP::GlobalSettings::Get<SP::GlobalSettings::Setting::LogFileRetention>();
    if (logFileRetention == 0) {
        return false;
    }

    OSTime oneDay = OSSecondsToTicks(86400ll);
    OSTime retentionDays = oneDay * logFileRetention;

    return tick + retentionDays <= OSGetTime();
}

static void RemoveOldLogFiles() {
    auto dir = Storage::OpenDir(LOG_FILE_DIRECTORY);
    if (!dir) {
        return;
    }

    while (auto nodeInfo = dir->read()) {
        if (!IsValidLogFile(*nodeInfo)) {
            continue;
        }
        if (!ShouldDeleteLogFile(nodeInfo->tick)) {
            continue;
        }

        wchar_t logFilePath[48];
        swprintf(logFilePath, sizeof(logFilePath), LOG_FILE_DIRECTORY L"/%ls", nodeInfo->name);

        if (!Storage::Remove(logFilePath, true)) {
            SP_LOG("Failed to remove the log file '%ls'!", logFilePath);
        }
    }
}

static void Init() {
    startTime = OSGetTime();

    RemoveOldLogFiles();

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
    u32 length = snprintf(buffers[index] + offset, maxLength, "[%u.%03u] ", secs, msecs);
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
