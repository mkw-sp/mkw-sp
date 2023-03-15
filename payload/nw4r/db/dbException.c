#include <Common.h>
#include <sp/Host.h>

extern void Exception_Printf_(const char *, ...);
extern void Exception_PrintCtx();

static void line1(const char * /* str */, u32 frame) {
    Host_PrintMkwSpInfo(Exception_Printf_);
    Exception_Printf_("******** EXCEPTION OCCURRED! ********\nFrameMemory:%XH\n", frame);
}
static void line2(void) {
    return Exception_Printf_("--------------------------------\n");
}

PATCH_BL(Exception_PrintCtx + 0x48, line1);
PATCH_BL(Exception_PrintCtx + 0x84, line2);
