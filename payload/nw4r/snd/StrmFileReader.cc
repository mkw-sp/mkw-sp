#include "StrmFileReader.hh"

namespace nw4r::snd {

bool StrmFileReader::readStrmTrackInfo(void *trackInfo, u32 trackId) {
    if (REPLACED(readStrmTrackInfo)(trackInfo, trackId)) {
        return true;
    }

    return REPLACED(readStrmTrackInfo)(trackInfo, 0);
}

} // namespace nw4r::snd
