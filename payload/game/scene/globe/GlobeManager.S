#include <Common.S>

// Do not initialize the globe heap here

PATCH_REPLACE_START(GlobeManager_init, 0x2c)
    b 0x18
PATCH_REPLACE_END(GlobeManager_init, 0x2c)



// Do not deinitialize the globe heap here

PATCH_B_START(GlobeManager_dt, 0x48)
    bl ResourceManager_flush
    b GlobeManager_dt + 0x4c
PATCH_B_END(GlobeManager_dt, 0x48)
