#include "FatStorage.h"

#include "ScopeLock.h"
#include "Sdi.h"

#include <ff/ff.h>
#include <revolution.h>

#include <string.h>

enum {
    MAX_OPEN_FILE_COUNT = 32,
    MAX_OPEN_DIR_COUNT = 32,
};

static OSMutex mutex;
static FatStorage fatStorage;
static FATFS fs;
static u32 openFiles = 0;
static FIL files[MAX_OPEN_FILE_COUNT];
static u32 openDirs = 0;
static DIR dirs[MAX_OPEN_DIR_COUNT];

static bool FatStorage_open(File *file, const wchar_t *path, u32 mode) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    for (file->fd = 0; file->fd < MAX_OPEN_FILE_COUNT; file->fd++) {
        if (!(openFiles & 1 << file->fd)) {
            break;
        }
    }
    if (file->fd == MAX_OPEN_FILE_COUNT) {
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
    if (f_open(&files[file->fd], path, fMode) != FR_OK) {
        return false;
    }

    openFiles |= 1 << file->fd;

    return true;
}

static bool FatStorage_close(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    openFiles &= ~(1 << file->fd);
    return f_close(&files[file->fd]) == FR_OK;
}

static bool FatStorage_read(File *file, void *dst, u32 size, u32 offset, u32 *readSize) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    if (f_lseek(&files[file->fd], offset) != FR_OK) {
        return false;
    }

    return f_read(&files[file->fd], dst, size, (UINT *)readSize) == FR_OK;
}

static bool FatStorage_write(File *file, const void *src, u32 size, u32 offset, u32 *writtenSize) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    if (f_lseek(&files[file->fd], offset) != FR_OK) {
        return false;
    }

    return f_write(&files[file->fd], src, size, (UINT *)writtenSize) == FR_OK;
}

static bool FatStorage_sync(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    return f_sync(&files[file->fd]) == FR_OK;
}

static u64 FatStorage_size(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    return f_size(&files[file->fd]);
}

static bool FatStorage_createDir(const wchar_t *path, bool allowNop) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    FRESULT fResult = f_mkdir(path);
    return fResult == FR_OK || (allowNop && fResult == FR_EXIST);
}

static bool FatStorage_openDir(Dir *dir, const wchar_t *path) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    for (dir->fd = 0; dir->fd < MAX_OPEN_DIR_COUNT; dir->fd++) {
        if (!(openDirs & 1 << dir->fd)) {
            break;
        }
    }
    if (dir->fd == MAX_OPEN_DIR_COUNT) {
        return false;
    }

    if (f_opendir(&dirs[dir->fd], path) != FR_OK) {
        return false;
    }

    openDirs |= 1 << dir->fd;

    return true;
}

static bool FatStorage_readDir(Dir *dir, DirEntry *entry) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    FILINFO info;
    if (f_readdir(&dirs[dir->fd], &info) != FR_OK) {
        return false;
    }

    static_assert(sizeof(info.fname) <= sizeof(entry->name));
    memcpy(entry->name, info.fname, sizeof(info.fname));
    if (info.fname[0] == L'\0') {
        entry->type = NODE_TYPE_NONE;
    } else if (info.fattrib & AM_DIR) {
        entry->type = NODE_TYPE_DIR;
    } else {
        entry->type = NODE_TYPE_FILE;
    }

    return true;
}

static bool FatStorage_closeDir(Dir *dir) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    openDirs &= ~(1 << dir->fd);
    return f_closedir(&dirs[dir->fd]) == FR_OK;
}

static u32 FatStorage_type(const wchar_t *path) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    u32 type = NODE_TYPE_NONE;
    FILINFO info;
    if (f_stat(path, &info) == FR_OK) {
        if (info.fattrib & AM_DIR) {
            type = NODE_TYPE_DIR;
        } else {
            type = NODE_TYPE_FILE;
        }
    }

    return type;
}

static bool FatStorage_rename(const wchar_t *srcPath, const wchar_t *dstPath) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    return f_rename(srcPath, dstPath) == FR_OK;
}

static bool FatStorage_delete(const wchar_t *path, bool allowNop) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    FRESULT fResult = f_unlink(path);
    return fResult == FR_OK || (allowNop && fResult == FR_NO_FILE);
}

static bool FatStorage_find(void) {
    if (SdiStorage_init(&fatStorage)) {
        return true;
    }

    return false;
}

bool FatStorage_init(Storage *storage) {
    OSInitMutex(&mutex);

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
    storage->sync = FatStorage_sync;
    storage->size = FatStorage_size;
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
