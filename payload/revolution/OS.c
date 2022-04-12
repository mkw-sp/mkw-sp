#include "revolution/os.h"

#include "sp/Payload.h"

PATCH_B(OSInit + 0x474, Payload_init);
