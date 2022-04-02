#include "revolution/os.h"

#include "game/host_system/Payload.h"

PATCH_B(OSInit + 0x474, Payload_init);
