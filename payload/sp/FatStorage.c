#include "FatStorage.h"

#include "ScopeLock.h"
#include "Sdi.h"
#include "UsbStorage.h"

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

static bool findFd(u32 *fd, u32 open, u32 max) {
    for (*fd = 0; *fd < max; (*fd)++) {
        if (!(open & 1 << *fd)) {
            return true;
        }
    }

    return false;
}

static bool FatStorage_fastOpen(File *file, u64 id) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (!findFd(&file->fd, openFiles, MAX_OPEN_FILE_COUNT)) {
        return false;
    }

    if (f_fastopen(&files[file->fd], &fs, id) != FR_OK) {
        return false;
    }

    openFiles |= 1 << file->fd;

    return true;
}

static bool FatStorage_open(File *file, const wchar_t *path, const char *mode) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (!findFd(&file->fd, openFiles, MAX_OPEN_FILE_COUNT)) {
        return false;
    }

    u32 fMode;
    if (!strcmp(mode, "r")) {
        fMode = FA_READ;
    } else if (!strcmp(mode, "w")) {
        fMode = FA_CREATE_ALWAYS | FA_WRITE;
    } else if (!strcmp(mode, "wx")) {
        fMode = FA_CREATE_NEW | FA_WRITE;
    } else {
        assert(!"Unknown opening mode");
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

static bool FatStorage_read(File *file, void *dst, u32 size, u32 offset) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    if (f_lseek(&files[file->fd], offset) != FR_OK) {
        return false;
    }

    UINT readSize;
    if (f_read(&files[file->fd], dst, size, &readSize) != FR_OK) {
        return false;
    }

    return readSize == size;
}

static bool FatStorage_write(File *file, const void *src, u32 size, u32 offset) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    if (f_lseek(&files[file->fd], offset) != FR_OK) {
        return false;
    }

    UINT writtenSize;
    if (f_write(&files[file->fd], src, size, &writtenSize) != FR_OK) {
        return false;
    }

    return writtenSize == size;
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

static bool FatStorage_fastOpenDir(Dir *dir, u64 id) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (!findFd(&dir->fd, openDirs, MAX_OPEN_DIR_COUNT)) {
        return false;
    }

    if (f_fastopendir(&dirs[dir->fd], &fs, id) != FR_OK) {
        return false;
    }

    openDirs |= 1 << dir->fd;

    return true;
}

static bool FatStorage_openDir(Dir *dir, const wchar_t *path) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (!findFd(&dir->fd, openDirs, MAX_OPEN_DIR_COUNT)) {
        return false;
    }

    if (f_opendir(&dirs[dir->fd], path) != FR_OK) {
        return false;
    }

    openDirs |= 1 << dir->fd;

    return true;
}

static bool FatStorage_readDir(Dir *dir, NodeInfo *info) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    FILINFO fInfo;
    if (f_readdir(&dirs[dir->fd], &fInfo) != FR_OK) {
        return false;
    }

    info->id.id = fInfo.dir_ofs;
    if (fInfo.fname[0] == L'\0') {
        info->type = NODE_TYPE_NONE;
    } else if (fInfo.fattrib & AM_DIR) {
        info->type = NODE_TYPE_DIR;
    } else {
        info->type = NODE_TYPE_FILE;
    }
    static_assert(sizeof(fInfo.fname) <= sizeof(info->name));
    memcpy(info->name, fInfo.fname, sizeof(fInfo.fname));

    return true;
}

static bool FatStorage_closeDir(Dir *dir) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    openDirs &= ~(1 << dir->fd);
    return f_closedir(&dirs[dir->fd]) == FR_OK;
}

static void FatStorage_stat(const wchar_t *path, NodeInfo *info) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    info->type = NODE_TYPE_NONE;
    FILINFO fInfo;
    if (f_stat(path, &fInfo) == FR_OK) {
        info->id.id = fInfo.dir_ofs;
        if (fInfo.fattrib & AM_DIR) {
            info->type = NODE_TYPE_DIR;
        } else {
            info->type = NODE_TYPE_FILE;
        }
        info->size = fInfo.fsize;
        static_assert(sizeof(fInfo.fname) <= sizeof(info->name));
        memcpy(info->name, fInfo.fname, sizeof(fInfo.fname));
    }
}

static bool FatStorage_rename(const wchar_t *srcPath, const wchar_t *dstPath) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    return f_rename(srcPath, dstPath) == FR_OK;
}

static bool FatStorage_remove(const wchar_t *path, bool allowNop) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    FRESULT fResult = f_unlink(path);
    return fResult == FR_OK || (allowNop && fResult == FR_NO_FILE);
}

static bool FatStorage_find(void) {
    if (UsbStorage_init(&fatStorage)) {
        return true;
    }

    if (SdiStorage_init(&fatStorage)) {
        return true;
    }

    return false;
}

bool FatStorage_init(Storage *storage) {
    OSInitMutex(&mutex);

    if (!FatStorage_find()) {
        SP_LOG("Failed to find a suitable device");
        return false;
    }

    if (f_mount(&fs, L"", 1) != FR_OK) {
        SP_LOG("Failed to mount the filesystem");
        return false;
    }

    storage->fastOpen = FatStorage_fastOpen;
    storage->open = FatStorage_open;
    storage->close = FatStorage_close;
    storage->read = FatStorage_read;
    storage->write = FatStorage_write;
    storage->sync = FatStorage_sync;
    storage->size = FatStorage_size;
    storage->createDir = FatStorage_createDir;
    storage->fastOpenDir = FatStorage_fastOpenDir;
    storage->openDir = FatStorage_openDir;
    storage->readDir = FatStorage_readDir;
    storage->closeDir = FatStorage_closeDir;
    storage->stat = FatStorage_stat;
    storage->rename = FatStorage_rename;
    storage->remove = FatStorage_remove;

    SP_LOG("Successfully completed initialization");
    return true;
}

u32 FatStorage_diskSectorSize(void) {
    return fatStorage.diskSectorSize();
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
