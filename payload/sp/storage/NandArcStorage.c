#include "NandArcStorage.h"

#include "sp/ScopeLock.h"

#include <common/Paths.h>

#include <stdalign.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

enum {
    MAX_OPEN_FILE_COUNT = 32,
    MAX_OPEN_DIR_COUNT = 32,
};

static OSMutex mutex;
static NANDFileInfo fileInfo;
static void *metadata;
static ARCHandle handle;
static u32 openFiles = 0;
static ARCFileInfo files[MAX_OPEN_FILE_COUNT];
static u32 openDirs = 0;
static ARCDir dirs[MAX_OPEN_DIR_COUNT];
static char pathBuffer[128];

static bool NandArcStorage_findFd(u32 *fd, u32 open, u32 max) {
    for (*fd = 0; *fd < max; (*fd)++) {
        if (!(open & 1 << *fd)) {
            return true;
        }
    }

    return false;
}

static const char *NandArcStorage_convertPath(const wchar_t *path) {
    if (!wcsncmp(path, L"/mkw-sp/disc/", wcslen(L"/mkw-sp/disc/"))) {
        snprintf(pathBuffer, sizeof(pathBuffer), "%ls", path + wcslen(L"/mkw-sp/disc/"));
        return pathBuffer;
    }

    return NULL;
}

static bool NandArcStorage_fastOpen(File *file, u64 id) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (!NandArcStorage_findFd(&file->fd, openFiles, MAX_OPEN_FILE_COUNT)) {
        return false;
    }

    if (!ARCFastOpen(&handle, (s32)id, &files[file->fd])) {
        return false;
    }

    openFiles |= 1 << file->fd;

    return true;
}

static bool NandArcStorage_open(File *file, const wchar_t *path, const char *mode) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (strcmp(mode, "r")) {
        return false;
    }

    if (!NandArcStorage_findFd(&file->fd, openFiles, MAX_OPEN_FILE_COUNT)) {
        return false;
    }

    const char *filePath = NandArcStorage_convertPath(path);
    if (!filePath) {
        return false;
    }

    if (!ARCOpen(&handle, filePath, &files[file->fd])) {
        return false;
    }

    openFiles |= 1 << file->fd;

    return true;
}

static bool NandArcStorage_close(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    openFiles &= ~(1 << file->fd);
    return ARCClose(&files[file->fd]);
}

static bool NandArcStorage_read(File *file, void *dst, u32 size, u32 offset) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    s32 fileOffset = ARCGetStartOffset(&files[file->fd]);
    assert(fileOffset >= 0);
    offset += fileOffset;
    assert(offset < INT32_MAX);
    if (NANDSeek(&fileInfo, offset, NAND_SEEK_SET) != (s32)offset) {
        return false;
    }

    s32 fileSize = ARCGetLength(&files[file->fd]);
    assert(fileSize >= 0);
    if (size > (u32)fileSize) {
        return false;
    }
    assert(size < INT32_MAX);
    return NANDRead(&fileInfo, dst, size) == (s32)size;
}

static bool NandArcStorage_write(File *UNUSED(file), const void *UNUSED(src), u32 UNUSED(size),
        u32 UNUSED(offset)) {
    return false;
}

static bool NandArcStorage_sync(File *UNUSED(file)) {
    return false;
}

static u64 NandArcStorage_size(File *file) {
    assert(file->fd < MAX_OPEN_FILE_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    return ARCGetLength(&files[file->fd]);
}

static bool NandArcStorage_createDir(const wchar_t *UNUSED(path), bool UNUSED(allowNop)) {
    return false;
}

static bool NandArcStorage_rename(const wchar_t *UNUSED(srcPath), const wchar_t *UNUSED(dstPath)) {
    return false;
}

static bool NandArcStorage_remove(const wchar_t *UNUSED(path), bool UNUSED(allowNop)) {
    return false;
}

static bool NandArcStorage_fastOpenDir(Dir *dir, u64 id) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (!NandArcStorage_findFd(&dir->fd, openDirs, MAX_OPEN_DIR_COUNT)) {
        return false;
    }

    if (!ARCFastOpenDir(&handle, (s32)id, &dirs[dir->fd])) {
        return false;
    }

    openDirs |= 1 << dir->fd;

    return true;
}

static bool NandArcStorage_openDir(Dir *dir, const wchar_t *path) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    if (!NandArcStorage_findFd(&dir->fd, openDirs, MAX_OPEN_DIR_COUNT)) {
        return false;
    }

    const char *dirPath = NandArcStorage_convertPath(path);
    if (!dirPath) {
        return false;
    }

    if (!ARCOpenDir(&handle, dirPath, &dirs[dir->fd])) {
        return false;
    }

    openDirs |= 1 << dir->fd;

    return true;
}

static bool NandArcStorage_readDir(Dir *dir, NodeInfo *info) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    ARCDirEntry entry;
    if (!ARCReadDir(&dirs[dir->fd], &entry)) {
        info->type = NODE_TYPE_NONE;
        return true;
    }

    info->id.id = entry.entryNum;
    swprintf(info->name, ARRAY_SIZE(info->name), L"%s", entry.name);

    if (entry.isDir) {
        info->type = NODE_TYPE_DIR;
        return true;
    }

    ARCFileInfo file;
    if (!ARCFastOpen(&handle, entry.entryNum, &file)) {
        return false;
    }
    info->size = ARCGetLength(&file);
    return ARCClose(&file);
}

static bool NandArcStorage_closeDir(Dir *dir) {
    assert(dir->fd < MAX_OPEN_DIR_COUNT);

    SP_SCOPED_MUTEX_LOCK(mutex);

    openDirs &= ~(1 << dir->fd);
    return ARCCloseDir(&dirs[dir->fd]);
}

static void NandArcStorage_stat(const wchar_t *path, NodeInfo *info) {
    SP_SCOPED_MUTEX_LOCK(mutex);

    info->type = NODE_TYPE_NONE;

    const char *filePath = NandArcStorage_convertPath(path);
    if (!filePath) {
        return;
    }
    s32 entrynum = ARCConvertPathToEntrynum(&handle, filePath);
    if (entrynum < 0) {
        return;
    }

    info->id.id = entrynum;
    swprintf(info->name, ARRAY_SIZE(info->name), L"%ls", wcsrchr(path, L'/') + 1);

    ARCFileInfo file;
    if (ARCFastOpen(&handle, entrynum, &file)) {
        info->type = NODE_TYPE_FILE;
        info->size = ARCGetLength(&file);
        ARCClose(&file);
        return;
    }

    ARCDir dir;
    if (ARCFastOpenDir(&handle, entrynum, &dir)) {
        info->type = NODE_TYPE_DIR;
        ARCCloseDir(&dir);
        return;
    }
}

bool NandArcStorage_init(Storage *storage) {
    OSInitMutex(&mutex);

    const char *tmpPath = "/tmp/contents.arc";
    const char *path = TITLE_DATA_PATH "/contents.arc";
    if (NANDOpen(tmpPath, &fileInfo, NAND_ACCESS_READ) != NAND_RESULT_OK &&
            NANDPrivateOpen(path, &fileInfo, NAND_ACCESS_READ) != NAND_RESULT_OK) {
        return false;
    }

    alignas(0x20) ARCHeader header;
    if (NANDRead(&fileInfo, &header, sizeof(header)) != sizeof(header)) {
        NANDClose(&fileInfo);
        return false;
    }

    metadata = OSAllocFromMEM1ArenaLo(header.fileStart, 0x20);
    if (!metadata) {
        NANDClose(&fileInfo);
        return false;
    }
    if (NANDSeek(&fileInfo, 0, NAND_SEEK_SET) != 0) {
        NANDClose(&fileInfo);
        return false;
    }
    if (NANDRead(&fileInfo, metadata, header.fileStart) != header.fileStart) {
        NANDClose(&fileInfo);
        return false;
    }

    if (!ARCInitHandle(metadata, &handle)) {
        NANDClose(&fileInfo);
        return false;
    }

    storage->fastOpen = NandArcStorage_fastOpen;
    storage->open = NandArcStorage_open;
    storage->close = NandArcStorage_close;
    storage->read = NandArcStorage_read;
    storage->write = NandArcStorage_write;
    storage->sync = NandArcStorage_sync;
    storage->size = NandArcStorage_size;
    storage->createDir = NandArcStorage_createDir;
    storage->fastOpenDir = NandArcStorage_fastOpenDir;
    storage->openDir = NandArcStorage_openDir;
    storage->readDir = NandArcStorage_readDir;
    storage->closeDir = NandArcStorage_closeDir;
    storage->stat = NandArcStorage_stat;
    storage->rename = NandArcStorage_rename;
    storage->remove = NandArcStorage_remove;

    return true;
}
