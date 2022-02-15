#include <Common.h>

extern void RkSystem_run;

// Don't create a heap for the rel because we already allocate some space from the arena for it. The
// function we are patching is still RkSystem_initialize but the NTSC-K offset is different so as a
// workaround we reference it from the next function instead.
static void *getNull(void) {
    return NULL;
}
PATCH_BL(RkSystem_run - 0x28, getNull);
