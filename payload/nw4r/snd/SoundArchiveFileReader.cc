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

u32 SoundArchiveFileReader::getSoundCount() const {
    u32 offset = m_info->soundTableOffset;
    auto *table = reinterpret_cast<Table *>(reinterpret_cast<u8 *>(m_info) + offset);
    return table->count;
}

} // namespace nw4r::snd
