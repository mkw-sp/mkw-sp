#include "SoundArchiveFileReader.hh"

namespace nw4r::snd {

u32 SoundArchiveFileReader::symbolDataOffset() const {
    return m_header.symbolDataOffset;
}

u32 SoundArchiveFileReader::symbolDataSize() const {
    return m_header.symbolDataSize;
}

u32 SoundArchiveFileReader::infoOffset() const {
    return m_header.infoOffset;
}

u32 SoundArchiveFileReader::infoSize() const {
    return m_header.infoSize;
}

} // namespace nw4r::snd
