#pragma once

#include <Common.hh>

namespace nw4r::snd {

class BasicSound {
public:
    void setPitch(f32 pitch);
    void REPLACED(setId)(u32 soundId);
    REPLACE void setId(u32 soundId);

    static s32 REPLACED(getAmbientPriority)(void *r3, u32 soundId);
    REPLACE static s32 getAmbientPriority(void *r3, u32 soundId);

private:
    u8 _000[0x100 - 0x000];
};

} // namespace nw4r::snd
