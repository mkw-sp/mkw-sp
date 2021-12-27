#include <Common.S>

PATCH_INJECT_START(CtrlRace2DMapCharacter_calcTransform, 0x138)
    # Check if the section id is ghost replay
    cmpwi r0, 0x34
    bne .Lend

    li r0, 0x6a # Pretend it's live view instead

.Lend:
    cmpwi r0, 0x6a # Original instruction
PATCH_INJECT_END(CtrlRace2DMapCharacter_calcTransform, 0x138)

# Make the icon of the selected player appear over the other ones
PATCH_INJECT_START(CtrlRace2DMapCharacter_calcTransform, 0x298)
    stfs f0, 0x48 (r3) # Original instruction

    lfs f3, 0x78 (r30)

    lfs f2, 0x7c (r28)
    fadds f2, f2, f3
    stfs f2, 0x7c (r28)

    lwz r3, 0x198 (r28)
    lfs f2, 0x34 (r3)
    fadds f2, f2, f3
    stfs f2, 0x34 (r3)
PATCH_INJECT_END(CtrlRace2DMapCharacter_calcTransform, 0x298)

# Fix copy-pasting bug
PATCH_REPLACE_START(CtrlRace2DMapCharacter_calcTransform, 0x5bc)
    lwz r3, 0x1c0 (r28)
PATCH_REPLACE_END(CtrlRace2DMapCharacter_calcTransform, 0x5bc)
