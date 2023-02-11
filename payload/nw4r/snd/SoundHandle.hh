#pragma once

#include "nw4r/snd/BasicSound.hh"

namespace nw4r::snd {

class SoundHandle {
public:
    SoundHandle();
    ~SoundHandle();

    void setPitch(f32 pitch);
    void setPan(f32 pan);

private:
    BasicSound *m_basicSound;
};

} // namespace nw4r::snd
