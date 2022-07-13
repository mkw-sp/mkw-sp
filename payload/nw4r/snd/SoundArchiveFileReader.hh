#pragma once

#include <Common.hh>

namespace nw4r::snd {

class SoundArchiveFileReader {
public:
    SoundArchiveFileReader();
    void init(void *header);
    void setStringChunk(void *stringChunk);
    void setInfoChunk(void *infoChunk);
    u32 symbolDataOffset() const;
    u32 symbolDataSize() const;
    u32 infoOffset() const;
    u32 infoSize() const;

private:
    struct Header {
        u8 _00[0x10 - 0x00];
        u32 symbolDataOffset;
        u32 symbolDataSize;
        u32 infoOffset;
        u32 infoSize;
        u8 _20[0x28 - 0x20];
    };
    static_assert(sizeof(Header) == 0x28);

    Header m_header;
    u8 _28[0x44 - 0x28];
};
static_assert(sizeof(SoundArchiveFileReader) == 0x44);

} // namespace nw4r::snd
