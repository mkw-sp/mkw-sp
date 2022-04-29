#pragma once

#include "nw4r/snd/snd_SoundArchive.h"

typedef struct {
    void *address;
    void *waveDataAddress;
} snd_FileAddress;
static_assert(sizeof(snd_FileAddress) == 0x8);

typedef struct {
    u32 count;
    snd_FileAddress entries[];
} snd_FileTable;

typedef struct {
    u8 _00[0x10 - 0x00];
    snd_SoundArchive *archive;
    snd_FileTable *fileTable; // Modified (groupTable originally)
    u8 _18[0xe0 - 0x18];
} snd_SoundArchivePlayer;
static_assert(sizeof(snd_SoundArchivePlayer) == 0xe0);

bool snd_SoundArchivePlayer_IsAvailable(const snd_SoundArchivePlayer *self);

bool snd_SoundArchivePlayer_LoadGroup(snd_SoundArchivePlayer *self, u32 groupId, void *r5, u32 r6);

void *snd_SoundArchivePlayer_GetFileAddress(snd_SoundArchivePlayer *self, u32 fileId);

void *snd_SoundArchivePlayer_GetFileWaveDataAddress(snd_SoundArchivePlayer *self, u32 fileId);
