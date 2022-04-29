#include <Common.S>

PATCH_BL_START(snd_SoundArchivePlayer_GetRequiredMemSize, 0xa0)
    b snd_SoundArchive_GetFileCount
PATCH_BL_END(snd_SoundArchivePlayer_GetRequiredMemSize, 0xa0)

PATCH_BL_START(snd_SoundArchivePlayer_SetupMram, 0x4c)
    b snd_SoundArchive_GetFileCount
PATCH_BL_END(snd_SoundArchivePlayer_SetupMram, 0x4c)

PATCH_BL_START(snd_SoundArchivePlayer_SetupMram, 0x84)
    b snd_SoundArchive_GetFileCount
PATCH_BL_END(snd_SoundArchivePlayer_SetupMram, 0x84)
