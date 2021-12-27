#include <Common.S>

// This function handles sound triggers. Normally they are only enabled for human players, except
// when the current section is a ghost replay one (0x32, 0x33, 0x34). This makes the game crash
// with multighost because the local player id can be -1 while the game assumes that it will be
// positive. To fix that this patch completely removes the exception (for now).
PATCH_REPLACE_START(Snd_80708b44, 0x28)
    blr
PATCH_REPLACE_END(Snd_80708b44, 0x28)
