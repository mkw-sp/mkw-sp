#include <Common.S>

// Fix use-after-free

PATCH_REPLACE_START(
        _ZN4nw4r3snd18SoundArchiveLoader9loadGroupEjPNS0_22SoundMemoryAllocatableEPvj, 0x38)
    li r6, 0x100
PATCH_REPLACE_END(
        _ZN4nw4r3snd18SoundArchiveLoader9loadGroupEjPNS0_22SoundMemoryAllocatableEPvj, 0x38)

PATCH_REPLACE_START(
        _ZN4nw4r3snd18SoundArchiveLoader9loadGroupEjPNS0_22SoundMemoryAllocatableEPvj, 0x2b0)
    addi r5, r31, 0x100
    li r6, 0x100
PATCH_REPLACE_END(
        _ZN4nw4r3snd18SoundArchiveLoader9loadGroupEjPNS0_22SoundMemoryAllocatableEPvj, 0x2b0)
