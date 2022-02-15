#include <Common.S>

PATCH_REPLACE_START(nw4r_db_Exception_Init, 0x34)
	li r0, 0x3F
PATCH_REPLACE_END(nw4r_db_Exception_Init, 0x34)
