#include "SoundHandle.hh"

namespace nw4r::snd {

void SoundHandle::setPitch(f32 pitch) {
    if (m_basicSound == nullptr) {
        return;
    }

    m_basicSound->setPitch(pitch);
}

} // namespace nw4r::snd
