#include "BasicSound.hh"

#include "nw4r/snd/SoundArchive.hh"

namespace nw4r::snd {

void BasicSound::setId(u32 soundId) {
    REPLACED(setId)(soundId & ~SASR_BIT);
}

s32 BasicSound::getAmbientPriority(void *r3, u32 soundId) {
    return REPLACED(getAmbientPriority)(r3, soundId & ~SASR_BIT);
}

} // namespace nw4r::snd
