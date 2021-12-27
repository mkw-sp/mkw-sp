#include "nand.h"

#include "isfs.h"

BOOL nandIsInitialized(void);

void nandGenerateAbsPath(char *absPath, const char *path);

BOOL nandIsPrivatePath(const char *absPath);

s32 nandConvertErrorCode(s32 code);

s32 NANDReadDir(const char *path, char *nameList, u32 *num) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    char absPath[0x40] = { 0 };
    nandGenerateAbsPath(absPath, path);

    if (nandIsPrivatePath(absPath)) {
        return nandConvertErrorCode(-102);
    }

    s32 code = ISFS_ReadDir(absPath, nameList, num);
    return nandConvertErrorCode(code);
}
