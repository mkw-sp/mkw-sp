#pragma once

extern "C" {
#include <revolution/os/OSThread.h>
}

namespace EGG {

class Thread {
public:
    static void REPLACED(switchThreadCallback)(OSThread *previousThread, OSThread *nextThread);
    static REPLACE void switchThreadCallback(OSThread *previousThread, OSThread *nextThread);
};

} // namespace EGG
