#pragma once

#include "nw4r/ut/ut_FileStream.h"

typedef struct {
    u32 fileSize;
    u32 waveDataFileSize;
    const char *extFilePath;
    u8 _0c[0x10 - 0x0c];
} snd_FileInfo;
static_assert(sizeof(snd_FileInfo) == 0x10);

typedef struct {
    u32 groupId;
    u32 index;
} snd_FilePos;
static_assert(sizeof(snd_FilePos) == 0x8);

typedef struct {
    u32 itemCount;
    u8 _04[0x08 - 0x04];
    u32 offset;
    u32 size;
    u32 waveDataOffset;
    u8 _14[0x18 - 0x14];
} snd_GroupInfo;
static_assert(sizeof(snd_GroupInfo) == 0x18);

typedef struct {
    u32 fileId;
    u32 offset;
    u8 _08[0x0c - 0x08];
    u32 waveDataOffset;
    u8 _10[0x14 - 0x10];
} snd_GroupItemInfo;
static_assert(sizeof(snd_GroupItemInfo) == 0x14);

typedef struct {
    struct snd_SoundArchive_vt *vt;
    u8 _004[0x008 - 0x004];
    char extFileRoot[256];
} snd_SoundArchive;
static_assert(sizeof(snd_SoundArchive) == 0x108);

typedef struct snd_SoundArchive_vt {
    u8 _00[0x18 - 0x00];
    ut_FileStream *(*OpenStream)(snd_SoundArchive *self, void *buffer, s32 size, u32 start,
            u32 length);
    ut_FileStream *(*OpenExtStream)(snd_SoundArchive *self, void *buffer, s32 size,
            const char *extFilePath, u32 start, u32 length);
} snd_SoundArchive_vt;
static_assert(sizeof(snd_SoundArchive_vt) == 0x20);

u32 snd_SoundArchive_GetFileCount(const snd_SoundArchive *self);

bool snd_SoundArchive_ReadFileInfo(snd_SoundArchive *self, u32 fileId, snd_FileInfo *fileInfo);

bool snd_SoundArchive_ReadFilePos(snd_SoundArchive *self, u32 fileId, u32 index,
        snd_FilePos *filePos);

u32 snd_SoundArchive_GetGroupCount(const snd_SoundArchive *self);

bool snd_SoundArchive_ReadGroupInfo(snd_SoundArchive *self, u32 groupId, snd_GroupInfo *groupInfo);

bool snd_SoundArchive_ReadGroupItemInfo(snd_SoundArchive *self, u32 groupId, u32 index,
        snd_GroupItemInfo *groupItemInfo);

ut_FileStream *snd_SoundArchive_OpenFileStream(snd_SoundArchive *self, u32 fileId, void *buffer,
        s32 size);

ut_FileStream *snd_SoundArchive_OpenFileWaveDataStream(snd_SoundArchive *self, u32 fileId,
        void *buffer, s32 size);
