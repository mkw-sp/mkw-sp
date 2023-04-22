#pragma once

#include <revolution.hh>

namespace EGG {

class Thread {
public:
    static void REPLACED(switchThreadCallback)(OSThread *previousThread, OSThread *nextThread);
    static REPLACE void switchThreadCallback(OSThread *previousThread, OSThread *nextThread);

    static void kandoTestCancelAllThread();
};

} // namespace EGG
