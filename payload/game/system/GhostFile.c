#include "GhostFile.h"

// Do not allocate memory for ghosts
PATCH_S16(GhostGroup_ct, 0x56, 0); // Saved
PATCH_S16(GhostGroup_ct, 0x62, 0); // Downloaded
PATCH_S16(GhostGroup_ct, 0x6e, 0); // Staff (per difficulty)
PATCH_S16(GhostGroup_ct, 0x7a, 0); // Competition
