#include "dvd.h"

typedef struct {
    u8 isDir : 8;
    u32 stringOffset : 24;
    union {
        struct {
            u32 _4;
            u32 next;
        } dir;
        struct {
            u32 _4;
            u32 _8;
        } file;
    };
} FstEntry;
static_assert(sizeof(FstEntry) == 0xc);

extern u32 MaxEntryNum;
extern char *FstStringStart;
extern FstEntry *FstStart;

BOOL DVDFastOpenDir(s32 entrynum, DVDDir *dir) {
    if (entrynum < 0) {
        return false;
    }

    if ((u32)entrynum >= MaxEntryNum) {
        return false;
    }

    if (!FstStart[entrynum].isDir) {
        return false;
    }

    dir->entryNum = entrynum;
    dir->location = entrynum + 1;
    dir->next = FstStart[entrynum].dir.next;
    return true;
}

BOOL DVDReadDir(DVDDir *dir, DVDDirEntry *dirent) {
    if (dir->location <= dir->entryNum) {
        return false;
    }

    if (dir->location >= dir->next) {
        return false;
    }

    dirent->entryNum = dir->location;
    dirent->isDir = !!FstStart[dir->location].isDir;
    dirent->name = FstStringStart + FstStart[dir->location].stringOffset;

    if (FstStart[dir->location].isDir) {
        dir->location = FstStart[dir->location].dir.next;
    } else {
        dir->location++;
    }

    return true;
}

BOOL DVDCloseDir(DVDDir *UNUSED(dir)) {
    return true;
}
