#include "GhostFile.h"

// Change the maximum number of ghosts depending on the type
PATCH_S16(GhostGroup_ct, 0x56, MAX_GHOST_COUNT); // Saved
PATCH_S16(GhostGroup_ct, 0x62, 0); // Downloaded
PATCH_S16(GhostGroup_ct, 0x6e, 0); // Staff (per difficulty)
PATCH_S16(GhostGroup_ct, 0x7a, 0); // Competition
