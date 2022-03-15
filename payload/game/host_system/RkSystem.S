#include <Common.S>

// Increase the number of lines that the exception console can hold
PATCH_REPLACE_START(RkSystem_initialize, 0x190)
	li r4, 110
PATCH_REPLACE_END(RkSystem_initialize, 0x190)

// Automatically display exception information on the television screen when an exception occurs
PATCH_REPLACE_START(RkSystem_initialize, 0x1A0)
	nop
	li r3, 0
PATCH_REPLACE_END(RkSystem_initialize, 0x1A0)
