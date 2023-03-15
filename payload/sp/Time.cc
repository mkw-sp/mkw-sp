#include "Time.hh"

extern "C" {
#include <revolution.h>
}

#include <optional>

namespace SP::Time {

static std::optional<u32> ReadRTC() {
    if (!EXILock(0, 1, NULL)) {
        return {};
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return {};
    }
    u32 cmd = 0x20000000;
    if (!EXIImm(0, &cmd, sizeof(cmd), EXI_WRITE, NULL)) {
        EXIDeselect(0);
        EXIUnlock(0);
        return {};
    }
    if (!EXISync(0)) {
        EXIDeselect(0);
        EXIUnlock(0);
        return {};
    }
    u32 time = 0;
    if (!EXIImm(0, &time, sizeof(time), EXI_READ, NULL)) {
        EXIDeselect(0);
        EXIUnlock(0);
        return {};
    }
    if (!EXISync(0)) {
        EXIDeselect(0);
        EXIUnlock(0);
        return {};
    }
    if (!EXIDeselect(0)) {
        EXIUnlock(0);
        return {};
    }
    if (!EXIUnlock(0)) {
        return {};
    }
    return time;
}

void Init() {
    u32 bias = SCGetCounterBias();
    std::optional<u32> rtcs[2] = {ReadRTC(), ReadRTC()};
    while (rtcs[0] != rtcs[1] || !rtcs[0]) {
        rtcs[0] = rtcs[1];
        rtcs[1] = ReadRTC();
    }
    OSSetTime(OSSecondsToTicks(static_cast<OSTime>(*rtcs[0] + bias)));
}

} // namespace SP::Time
