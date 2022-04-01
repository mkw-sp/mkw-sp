#include <Common.S>

// Replace DVDFastOpen with DVDOpen so we don't have to deal with fake entrynums for file
// replacement.

PATCH_REPLACE_START(snd_DvdSoundArchive_Open, 0x24)
    li r3, 0
PATCH_REPLACE_END(snd_DvdSoundArchive_Open, 0x24)

PATCH_BL_START(snd_DvdSoundArchive_Open, 0x3c)
    mr r3, r30
    b DVDOpen
PATCH_BL_END(snd_DvdSoundArchive_Open, 0x3c)

PATCH_BL_START(snd_DvdSoundArchive_OpenExtStream, 0x4c)
    mr r31, r6 // Because of the other patch, r31 is now safe to use
    b DVDExConvertPathToEntrynum
PATCH_BL_END(snd_DvdSoundArchive_OpenExtStream, 0x4c)

PATCH_REPLACE_START(snd_DvdSoundArchive_OpenExtStream, 0x54)
    mr r4, r31
PATCH_REPLACE_END(snd_DvdSoundArchive_OpenExtStream, 0x54)



// Ignore the stream size from the BRSAR

PATCH_B_START(snd_DvdSoundArchive_OpenExtStream, 0x14)
    lis r31, 0x7fff
    ori r31, r31, 0xffff

    b snd_DvdSoundArchive_OpenExtStream + 0x18
PATCH_B_END(snd_DvdSoundArchive_OpenExtStream, 0x14)
