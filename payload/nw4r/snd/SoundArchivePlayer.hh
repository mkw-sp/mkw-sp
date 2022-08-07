#pragma once

#include "nw4r/snd/SoundArchiveLoader.hh"
#include "nw4r/snd/SoundHandle.hh"
#include "nw4r/snd/SoundMemoryAllocatable.hh"

namespace nw4r::snd {

class SoundArchivePlayer {
public:
    bool isAvailable() const;
    u32 REPLACED(setupSoundImpl)(SoundHandle *handle, u32 soundId, void *r6, void *r7, bool r8,
            void *r9);
    REPLACE u32 setupSoundImpl(SoundHandle *handle, u32 soundId, void *r6, void *r7, bool r8,
            void *r9);
    bool REPLACED(loadGroup)(u32 groupId, SoundMemoryAllocatable *allocator, u32 r6);
    REPLACE bool loadGroup(u32 groupId, SoundMemoryAllocatable *allocator, u32 r6);
    void REPLACED(invalidateData)(const void *start, const void *end);
    REPLACE void invalidateData(const void *start, const void *end);

private:
    struct FileAddress {
        void *address;
        void *waveDataAddress;
    };
    static_assert(sizeof(FileAddress) == 0x8);

    struct FileTable {
        u32 count;
        FileAddress entries[];
    };

    ut::FileStream **soundStreams();
    u32 REPLACED(getRequiredMemSize)(SoundArchive *archive);
    REPLACE u32 getRequiredMemSize(SoundArchive *archive);
    bool REPLACED(setupMram)(SoundArchive *archive, void *buffer, u32 bufferSize);
    REPLACE bool setupMram(SoundArchive *archive, void *buffer, u32 bufferSize);
    u32 REPLACED(prepareStrmImpl)(void *r4, SoundArchive::SoundInfo *soundInfo, void *r6, u32 r7,
            s32 r8);
    REPLACE u32 prepareStrmImpl(void *r4, SoundArchive::SoundInfo *soundInfo, void *r6, u32 r7,
            s32 r8);

    u8 _00[0x10 - 0x00];
    SoundArchive *m_archive;
    u8 _14[0xd8 - 0x14];
    void *m_buffer;
    u32 m_bufferSize;
};
static_assert(sizeof(SoundArchivePlayer) == 0xe0);

} // namespace nw4r::snd
