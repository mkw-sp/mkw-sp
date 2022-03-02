#include "FatStorage.h"

#include "Sdi.h"

#include <ff/ff.h>
#include <revolution.h>

#include <string.h>
#include <revolution.h>

enum {
    MAX_OPEN_FILE_COUNT = 32,
    MAX_OPEN_DIR_COUNT = 32,
};

static FATFS fs;
static OSMutex mutex;
static u32 openFiles = 0;
static FIL files[MAX_OPEN_FILE_COUNT];
static u32 openDirs = 0;
static DIR dirs[MAX_OPEN_DIR_COUNT];
static FatStorage fatStorage;

static bool FatStorage_open(File *file, const wchar_t *path, u32 mode) {
    OSLockMutex(&mutex);

    for (file->fd = 0; file->fd < MAX_OPEN_FILE_COUNT; file->fd++) {
        if (!(openFiles & 1 << file->fd)) {
            break;
        }
    }
    if (file->fd == MAX_OPEN_FILE_COUNT) {
        OSUnlockMutex(&mutex);
        return false;
    }

    u32 fMode = 0;
    if (mode & MODE_READ) {
        fMode |= FA_READ;
    }
    if (mode & MODE_WRITE) {
        fMode |= FA_WRITE;
    }
    if (mode & MODE_CREATE_NEW) {
        fMode |= FA_CREATE_NEW;
    }
    if (mode & MODE_CREATE_ALWAYS) {
        fMode |= FA_CREATE_ALWAYS;
    }
    bool result = f_open(&files[file->fd], path, fMode) == FR_OK;

    if (result) {
        openFiles |= 1 << file->fd;
    }

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_close(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    OSLockMutex(&mutex);

    bool result = f_close(&files[file->fd]) == FR_OK;

    openFiles &= ~(1 << file->fd);

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_read(File *file, void *dst, u32 size, u32 *readSize) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    OSLockMutex(&mutex);

    bool result = f_read(&files[file->fd], dst, size, (UINT *)readSize) == FR_OK;

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_write(File *file, const void *src, u32 size, u32 *writtenSize) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    OSLockMutex(&mutex);

    bool result = f_write(&files[file->fd], src, size, (UINT *)writtenSize) == FR_OK;

    OSUnlockMutex(&mutex);

    return result;
}

static u64 FatStorage_size(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    OSLockMutex(&mutex);

    u64 size = f_size(&files[file->fd]);

    OSUnlockMutex(&mutex);

    return size;
}

static bool FatStorage_lseek(File *file, u64 offset) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    OSLockMutex(&mutex);

    bool result = f_lseek(&files[file->fd], offset) == FR_OK;

    OSUnlockMutex(&mutex);

    return result;
}

static u64 FatStorage_tell(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    OSLockMutex(&mutex);

    u64 offset = f_tell(&files[file->fd]);

    OSUnlockMutex(&mutex);

    return offset;
}

static bool FatStorage_createDir(const wchar_t *path, bool allowNop) {
    OSLockMutex(&mutex);

    FRESULT fResult = f_mkdir(path);
    bool result = fResult == FR_OK || (allowNop && fResult == FR_EXIST);

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_openDir(Dir *dir, const wchar_t *path) {
    OSLockMutex(&mutex);

    for (dir->fd = 0; dir->fd < MAX_OPEN_DIR_COUNT; dir->fd++) {
        if (!(openDirs & 1 << dir->fd)) {
            break;
        }
    }
    if (dir->fd == MAX_OPEN_DIR_COUNT) {
        OSUnlockMutex(&mutex);
        return false;
    }

    bool result = f_opendir(&dirs[dir->fd], path) == FR_OK;

    if (result) {
        openDirs |= 1 << dir->fd;
    }

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_readDir(Dir *dir, DirEntry *entry) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    OSLockMutex(&mutex);

    FILINFO info;
    bool result = f_readdir(&dirs[dir->fd], &info) == FR_OK;

    if (result) {
        static_assert(sizeof(info.fname) <= sizeof(entry->name));
        memcpy(entry->name, info.fname, sizeof(info.fname));
        if (info.fname[0] == L'\0') {
            entry->type = NODE_TYPE_NONE;
        } else if (info.fattrib & AM_DIR) {
            entry->type = NODE_TYPE_DIR;
        } else {
            entry->type = NODE_TYPE_FILE;
        }
    }

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_closeDir(Dir *dir) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    OSLockMutex(&mutex);

    bool result = f_closedir(&dirs[dir->fd]) == FR_OK;

    openDirs &= ~(1 << dir->fd);

    OSUnlockMutex(&mutex);

    return result;
}

static u32 FatStorage_type(const wchar_t *path) {
    OSLockMutex(&mutex);

    u32 type = NODE_TYPE_NONE;
    FILINFO info;
    if (f_stat(path, &info) == FR_OK) {
        if (info.fattrib & AM_DIR) {
            type = NODE_TYPE_DIR;
        } else {
            type = NODE_TYPE_FILE;
        }
    }

    OSUnlockMutex(&mutex);

    return type;
}

static bool FatStorage_rename(const wchar_t *srcPath, const wchar_t *dstPath) {
    OSLockMutex(&mutex);

    bool result = f_rename(srcPath, dstPath) == FR_OK;

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_delete(const wchar_t *path, bool allowNop) {
    OSLockMutex(&mutex);

    FRESULT fResult = f_unlink(path);
    bool result = fResult == FR_OK || (allowNop && fResult == FR_NO_FILE);

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_find(void) {
    if (SdiStorage_init(&fatStorage)) {
        return true;
    }

    return false;
}

bool FatStorage_init(Storage *storage) {
    if (!FatStorage_find()) {
        SP_LOG("[FatStorage] Failed to find");
        return false;
    }

    if (f_mount(&fs, L"", 1) != FR_OK) {
        SP_LOG("[FatStorage] Failed to mount");
        return false;
    }

    storage->open = FatStorage_open;
    storage->close = FatStorage_close;
    storage->read = FatStorage_read;
    storage->write = FatStorage_write;
    storage->size = FatStorage_size;
    storage->lseek = FatStorage_lseek;
    storage->tell = FatStorage_tell;
    storage->createDir = FatStorage_createDir;
    storage->openDir = FatStorage_openDir;
    storage->readDir = FatStorage_readDir;
    storage->closeDir = FatStorage_closeDir;
    storage->type = FatStorage_type;
    storage->rename = FatStorage_rename;
    storage->delete = FatStorage_delete;

    SP_LOG("[FatStorage] Initialized");
    return true;
}

bool FatStorage_diskRead(u32 firstSector, u32 sectorCount, void *buffer) {
    return fatStorage.diskRead(firstSector, sectorCount, buffer);
}

bool FatStorage_diskWrite(u32 firstSector, u32 sectorCount, const void *buffer) {
    return fatStorage.diskWrite(firstSector, sectorCount, buffer);
}

bool FatStorage_diskErase(u32 firstSector, u32 sectorCount) {
    return fatStorage.diskErase(firstSector, sectorCount);
}

bool FatStorage_diskSync(void) {
    return fatStorage.diskSync();
}
