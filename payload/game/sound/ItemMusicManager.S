#include <Common.S>

PATCH_B_START(_ZN5Sound16ItemMusicManager4calcEv, 0x174)
    mr r30, r3
    mr r3, r29
    bl _ZNK5Sound16ItemMusicManager5pitchEv
    mr r3, r30
    li r4, 2
    b _ZN5Sound16ItemMusicManager4calcEv + 0x17c
PATCH_B_END(_ZN5Sound16ItemMusicManager4calcEv, 0x174)

