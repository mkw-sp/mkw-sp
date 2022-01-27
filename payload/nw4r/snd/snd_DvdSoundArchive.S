#include <Common.S>

PATCH_B_START(DvdSoundArchive_OpenExtStream, 0x14)
    lis r31, 0x7fff
    ori r31, r31, 0xffff

    b DvdSoundArchive_OpenExtStream + 0x18
PATCH_B_END(DvdSoundArchive_OpenExtStream, 0x14)
