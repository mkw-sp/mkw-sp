#pragma once

#include <Common.hh>

namespace nw4r::snd {

class BasicSound {
public:
    void setPitch(f32 pitch);

private:
    u8 _000[0x100 - 0x000];
};

} // namespace nw4r::snd
