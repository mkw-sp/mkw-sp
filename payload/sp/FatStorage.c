#include "FatStorage.h"

#include "Sdi.h"

#include <ff/ff.h>
#include <revolution.h>

enum {
    MAX_OPEN_FILE_COUNT = 32,
};

FATFS fs;
static OSMutex mutex;
u32 openedFiles = 0;
FIL files[MAX_OPEN_FILE_COUNT];
static FatStorage fatStorage;

static bool FatStorage_open(File *file, const char *path, u32 mode) {
    OSLockMutex(&mutex);

    for (file->fd = 0; file->fd < MAX_OPEN_FILE_COUNT; file->fd++) {
        if (!(openedFiles & 1 << file->fd)) {
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
    bool result = f_open(&files[file->fd], path, fMode) == FR_OK;

    if (result) {
        openedFiles |= 1 << file->fd;
    }

    OSUnlockMutex(&mutex);

    return result;
}

static bool FatStorage_close(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    OSLockMutex(&mutex);

    bool result = f_close(&files[file->fd]) == FR_OK;

    openedFiles &= ~(1 << file->fd);

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

static bool FatStorage_find(void) {
    if (SdiStorage_init(&fatStorage)) {
        return true;
    }

    return false;
}

bool FatStorage_init(Storage *storage) {
    if (!FatStorage_find()) {
        return false;
    }

    if (f_mount(&fs, "", 1) != FR_OK) {
        return false;
    }

    storage->open = FatStorage_open;
    storage->close = FatStorage_close;
    storage->read = FatStorage_read;
    storage->write = FatStorage_write;
    storage->size = FatStorage_size;
    storage->lseek = FatStorage_lseek;
    storage->tell = FatStorage_tell;

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
