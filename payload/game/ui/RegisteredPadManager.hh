#pragma once

#include <Common.hh>

namespace UI {

class RegisteredPadManager {
public:
    void onCreateSection(bool r4);
    void setDriftIsAuto(u32 localPlayerId, bool driftIsAuto);
    void setFlags(u32 localPlayerId, u32 flags);
    void _8061B5A4();

private:
    u8 _00[0x5c - 0x00];
};
static_assert(sizeof(RegisteredPadManager) == 0x5c);

} // namespace UI
