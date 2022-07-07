extern "C" {
#include "revolution/os.h"
}

#include <sp/ScopeLock.hh>

extern "C" void OSSetTime(OSTime time) {
    SP::ScopeLock<SP::NoInterrupts> lock;

    *reinterpret_cast<OSTime *>(0x800030d8) += OSGetTime() - time;
    u32 hi = time >> 32;
    u32 lo = time;
    asm("mttbl %0; mttbu %1; mttbl %2" : : "r"(0), "r"(hi), "r"(lo));
    EXIProbeReset();
}
