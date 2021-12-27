#include <Common.h>

extern void RkSystem_initialize;

// Use the exception handler from the SDK
PATCH_NOP(RkSystem_initialize, 0x19c);

// Don't create a heap for the rel because we already allocate some space from
// the arena for it.
PATCH_NOP(RkSystem_initialize, 0x360);
PATCH_NOP(RkSystem_initialize, 0x364);
