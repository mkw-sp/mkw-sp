#include <Common.S>

// Do not load RKNetPacketCreator
PATCH_B_START(RaceScene_initSubsystems, 0x14c)
    b RaceScene_initSubsystems + 0x178
PATCH_B_END(RaceScene_initSubsystems, 0x14c)
