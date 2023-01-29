#pragma once

namespace nw4r::snd {

class SoundPlayer {
private:
    u8 _00[0x38 - 0x00];

public:
    f32 m_mainOutVolume;

private:
    u8 _3c[0x5c - 0x3c];
};
static_assert(sizeof(SoundPlayer) == 0x5c);

} // namespace nw4r::snd
