#include "dvd.h"

#include "dvdex.h"

typedef struct {
    u8 isDir : 8;
    u32 stringOffset : 24;
    union {
        struct {
            u32 _4;
            u32 next;
        } dir;
        struct {
            u32 startAddr;
            u32 length;
        } file;
    };
} FstEntry;
static_assert(sizeof(FstEntry) == 0xc);

extern u32 MaxEntryNum;
extern char *FstStringStart;
extern FstEntry *FstStart;

BOOL my_DVDOpen(const char *fileName, DVDFileInfo *fileInfo) {
    if (DVDExOpen(fileName, fileInfo)) {
        return true;
    }

    s32 entrynum = DVDConvertPathToEntrynum(fileName);
    if (entrynum < 0 || (u32)entrynum >= MaxEntryNum) {
        return false;
    }
    if (FstStart[entrynum].isDir) {
        return false;
    }

    fileInfo->startAddr = FstStart[entrynum].file.startAddr;
    fileInfo->length = FstStart[entrynum].file.length;
    fileInfo->callback = NULL;
    fileInfo->cb.state = 0;
    return true;
}
PATCH_B(DVDOpen, my_DVDOpen);

BOOL my_DVDFastOpen(s32 entrynum, DVDFileInfo *fileInfo) {
    const char *fileName = DVDExConvertEntrynumToPath(entrynum);
    return my_DVDOpen(fileName, fileInfo);
}
PATCH_B(DVDFastOpen, my_DVDFastOpen);
