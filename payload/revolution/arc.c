#include "arc.h"

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

BOOL ARCFastOpenDir(ARCHandle *handle, s32 entrynum, ARCDir *dir) {
    if (entrynum < 0) {
        return false;
    }

    if ((u32)entrynum > handle->entryNum) {
        return false;
    }

    FstEntry *entry = &((FstEntry *)handle->FSTStart)[entrynum];
    if (!entry->isDir) {
        return false;
    }

    dir->handle = handle;
    dir->entryNum = entrynum;
    dir->location = entrynum + 1;
    dir->next = entry->dir.next;
    return true;
}
