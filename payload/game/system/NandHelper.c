#include "NandHelper.h"

u32 NandHelper_readFile(const char *path, void *buf, u32 maxLength, u32 *length) {
    u32 result;

    NANDFileInfo fileInfo;
    result = NandHelper_open(path, &fileInfo, NAND_ACCESS_READ);
    if (result != RK_NAND_RESULT_OK) {
        return result;
    }

    result = NandHelper_getLength(&fileInfo, length);
    if (result != RK_NAND_RESULT_OK) {
        NandHelper_close(&fileInfo);
        return result;
    }

    if (*length > maxLength) {
        return RK_NAND_RESULT_ACCESS;
    }

    result = NandHelper_read(&fileInfo, buf, *length, 0);
    if (result != RK_NAND_RESULT_OK) {
        NandHelper_close(&fileInfo);
        return result;
    }

    return NandHelper_close(&fileInfo);
}

u32 NandHelper_readFromFile(const char *path, void *buf, u32 length, u32 offset) {
    u32 result;

    NANDFileInfo fileInfo;
    result = NandHelper_open(path, &fileInfo, NAND_ACCESS_READ);
    if (result != RK_NAND_RESULT_OK) {
        return result;
    }

    result = NandHelper_read(&fileInfo, buf, length, offset);
    if (result != RK_NAND_RESULT_OK) {
        NandHelper_close(&fileInfo);
        return result;
    }

    return NandHelper_close(&fileInfo);
}

u32 NandHelper_readDir(const char *path, char *nameList, u32 *num) {
    for (u32 try = 0; try < 3; try++) {
        switch (NANDReadDir(path, nameList, num)) {
        case NAND_RESULT_OK:
            return RK_NAND_RESULT_OK;
        case NAND_RESULT_ACCESS:
            return RK_NAND_RESULT_ACCESS;
        case NAND_RESULT_ALLOC_FAILED:
        case NAND_RESULT_BUSY:
            OSSleepMilliseconds(100);
            break;
        case NAND_RESULT_NOEXISTS:
            return RK_NAND_RESULT_NOEXISTS;
        default:
            return RK_NAND_RESULT_OTHER;
        }
    }

    return RK_NAND_RESULT_BUSY;
}

u32 NandHelper_writeFile(const char *path, const void *buf, u32 length) {
    u32 result;

    NANDFileInfo fileInfo;
    result = NandHelper_open(path, &fileInfo, NAND_ACCESS_WRITE);
    if (result != RK_NAND_RESULT_OK) {
        return result;
    }

    result = NandHelper_write(&fileInfo, buf, length, 0);
    if (result != RK_NAND_RESULT_OK) {
        NandHelper_close(&fileInfo);
        return result;
    }

    return NandHelper_close(&fileInfo);
}

u32 NandHelper_getHomeDir(char *path) {
    switch (NANDGetHomeDir(path)) {
    case NAND_RESULT_OK:
        return RK_NAND_RESULT_OK;
    default:
        return RK_NAND_RESULT_OTHER;
    }
}

u32 NandHelper_move(const char *path, const char *destDir) {
    for (u32 try = 0; try < 3; try++) {
        switch (NANDMove(path, destDir)) {
        case NAND_RESULT_OK:
            return RK_NAND_RESULT_OK;
        case NAND_RESULT_ACCESS:
            return RK_NAND_RESULT_ACCESS;
        case NAND_RESULT_ALLOC_FAILED:
        case NAND_RESULT_BUSY:
            OSSleepMilliseconds(100);
            break;
        case NAND_RESULT_NOEXISTS:
            return RK_NAND_RESULT_NOEXISTS;
        default:
            return RK_NAND_RESULT_OTHER;
        }
    }

    return RK_NAND_RESULT_BUSY;
}
