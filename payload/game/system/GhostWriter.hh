#pragma once

#include <Common.hh>

namespace System {

class GhostWriter {
public:
    void REPLACED(writeFrame)(u16 rawButtons, u8 rawStickX, u8 rawStickY, u32 rawTrick);
    REPLACE void writeFrame(u16 rawButtons, u8 rawStickX, u8 rawStickY, u32 rawTrick);

private:
    u8 _00[0x1c - 0x00];
};
static_assert(sizeof(GhostWriter) == 0x1c);

} // namespace System