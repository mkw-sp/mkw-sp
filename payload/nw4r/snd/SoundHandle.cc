#include "SoundHandle.hh"

namespace nw4r::snd {

void SoundHandle::setPitch(f32 pitch) {
    if (m_basicSound == nullptr) {
        return;
    }

    m_basicSound->setPitch(pitch);
}

void SoundHandle::setPan(f32 pan) {
    if (m_basicSound == nullptr) {
        return;
    }

    m_basicSound->setPan(pan);
}

} // namespace nw4r::snd
