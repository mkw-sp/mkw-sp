#include "nand.h"

BOOL nandIsInitialized(void);

s32 nandMove(const char *path, const char *destDir, void *callback, BOOL async, BOOL private);

s32 nandConvertErrorCode(s32 code);

s32 NANDPrivateMove(const char *path, const char *destDir) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    s32 code = nandMove(path, destDir, NULL, false, true);
    return nandConvertErrorCode(code);
}
