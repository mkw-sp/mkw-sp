#include <Common.h>

extern void RkSystem_initialize;
extern void RkSystem_run;

// Automatically display exception information on the television screen when an exception occurs
PATCH_NOP(RkSystem_initialize, 0x1A0);
PATCH_U32(RkSystem_initialize, 0x1A4, 0x38600000 /* li r3, 0 */);

// Don't create a heap for the rel because we already allocate some space from the arena for it. The
// function we are patching is still RkSystem_initialize but the NTSC-K offset is different so as a
// workaround we reference it from the next function instead.
static void *getNull(void) {
    return NULL;
}
PATCH_BL(RkSystem_run - 0x28, getNull);
