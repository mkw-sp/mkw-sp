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
    class FileStream : public ut::FileStream {
    public:
        FileStream(SP::Storage::FileHandle file, u32 start, u32 size);
        ~FileStream() override;
        void close() override;
        s32 read(void *dst, u32 size) override;
        bool canAsync() override;
        bool canRead() override;
        bool canWrite() override;
        u32 getOffsetAlign() override;
        u32 getSizeAlign() override;
        u32 getBufferAlign() override;
        u32 getSize() override;
        void seek(s32 offset, SeekOrigin origin) override;
        bool canSeek() override;
        bool canCancel() override;
        u32 tell() override;

    private:
        std::optional<SP::Storage::FileHandle> m_file;
        u32 m_start = 0;
        u32 m_size;
        u32 m_offset;
    };

    SoundArchiveFileReader m_fileReader;
    std::optional<SP::Storage::FileHandle> m_file; // Modified
    u8 _154[0x18c - 0x154]; // Unused
};
static_assert(sizeof(DVDSoundArchive) == 0x18c);

} // namespace nw4r::snd
