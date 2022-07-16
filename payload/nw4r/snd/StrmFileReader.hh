#pragma once

#include <Common.hh>

namespace nw4r::snd {

class StrmFileReader {
public:
    bool REPLACED(readStrmTrackInfo)(void *trackInfo, u32 trackId);
    REPLACE bool readStrmTrackInfo(void *trackInfo, u32 trackId);

private:
    u8 _0[0x8 - 0x0];
};

} // namespace nw4r::snd
