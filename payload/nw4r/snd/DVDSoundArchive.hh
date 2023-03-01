#pragma once

#include "nw4r/snd/SoundArchive.hh"

#include <sp/storage/Storage.hh>

namespace nw4r::snd {

class DVDSoundArchive : public SoundArchive {
public:
    REPLACE DVDSoundArchive();
    ~DVDSoundArchive() override;
    REPLACE void dt(s32 type) override;
    const void *getFileAddress(u32 fileId) override;
    const void *getWaveDataFileAddress(u32 fileId) override;
    size_t getRequiredStreamBufferSize() override;
    ut::FileStream *openStream(void *buffer, s32 size, u32 start, u32 length) override;
    ut::FileStream *openExtStream(void *buffer, s32 size, const char *extFilePath, u32 start,
            u32 length) override;

    REPLACE bool open(const char *filePath);
    REPLACE void close();
    REPLACE bool loadHeader(void *buffer, u32 size);
    REPLACE bool loadLabelStringData(void *buffer, u32 size);

private:
    SoundArchiveFileReader m_fileReader;
    std::optional<SP::Storage::FileHandle> m_file{}; // Modified
};
static_assert(sizeof(DVDSoundArchive) <= 0x18c);

} // namespace nw4r::snd
