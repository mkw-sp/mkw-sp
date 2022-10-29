#include <Common.S>

#define MILLISECONDS_TO_WAIT 125

PATCH_REPLACE_START(EGG_ExceptionCallBack_, 0x16C)
    cmplwi    r0, MILLISECONDS_TO_WAIT
PATCH_REPLACE_END(EGG_ExceptionCallBack_, 0x16C)
