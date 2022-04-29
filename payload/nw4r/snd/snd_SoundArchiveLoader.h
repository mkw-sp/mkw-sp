#pragma once

#include "nw4r/snd/snd_SoundArchive.h"

typedef struct {
    u8 _000[0x220 - 0x000];
} snd_SoundArchiveLoader;
static_assert(sizeof(snd_SoundArchiveLoader) == 0x220);

snd_SoundArchiveLoader *snd_SoundArchiveLoader_ct(snd_SoundArchiveLoader *self,
        snd_SoundArchive *archive);

void *snd_SoundArchiveLoader_LoadFile(snd_SoundArchiveLoader *self, u32 fileId, void *r5);

void *snd_SoundArchiveLoader_LoadWaveDataFile(snd_SoundArchiveLoader *self, u32 fileId, void *r5);
