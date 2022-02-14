#include <Common.h>

#include "nw4r/db/dbException.h"

PATCH_U32(nw4r_db_Exception_Init, 52, 0x3800003F /* li r0, 0x3F */);
