#pragma once

#include "nw4r/snd/SoundArchiveFileReader.hh"
#include "nw4r/ut/FileStream.hh"

namespace nw4r::snd {

class SoundArchive {
public:
    struct GroupInfo {
        u32 itemCount;
        u8 _04[0x08 - 0x04];
        u32 offset;
        u32 size;
        u32 waveDataOffset;
        u8 _14[0x18 - 0x14];
    };
    static_assert(sizeof(GroupInfo) == 0x18);

    struct GroupItemInfo {
        u32 fileId;
        u32 offset;
        u8 _08[0x0c - 0x08];
        u32 waveDataOffset;
        u8 _10[0x14 - 0x10];
    };
    static_assert(sizeof(GroupItemInfo) == 0x14);

    struct FileInfo {
        u32 fileSize;
        u32 waveDataFileSize;
        const char *extFilePath;
        u8 _0c[0x10 - 0x0c];
    };
    static_assert(sizeof(FileInfo) == 0x10);

    struct FilePos {
        u32 groupId;
        u32 index;
    };
    static_assert(sizeof(FilePos) == 0x8);

    SoundArchive();
    virtual ~SoundArchive();
    virtual void dt(s32 type);
    virtual const void *getFileAddress(u32 fileId) = 0;
    virtual const void *getWaveDataFileAddress(u32 fileId) = 0;
    virtual size_t getRequiredStreamBufferSize() = 0;
    virtual ut::FileStream *openStream(void *buffer, s32 size, u32 start, u32 length) = 0;
    virtual ut::FileStream *openExtStream(void *buffer, s32 size, const char *extFilePath,
            u32 start, u32 length) = 0;

protected:
    void setup(SoundArchiveFileReader *fileReader);
    void shutdown();

public:
    u32 REPLACED(getGroupCount)();
    REPLACE u32 getGroupCount();
    bool readGroupInfo(u32 groupId, GroupInfo *groupInfo);
    bool readGroupItemInfo(u32 groupId, u32 index, GroupItemInfo *groupItemInfo);
    u32 getFileCount();
    bool readFileInfo(u32 fileId, FileInfo *fileInfo);
    bool readFilePos(u32 fileId, u32 index, FilePos *filePos);
    REPLACE ut::FileStream *openFileStream(u32 fileId, void *buffer, s32 size);
    REPLACE ut::FileStream *openFileStreamWaveData(u32 fileId, void *buffer, s32 size);

protected:
    SoundArchiveFileReader *m_fileReader;
    char m_extFileRoot[256];
};
static_assert(sizeof(SoundArchive) == 0x108);

} // namespace nw4r::snd
