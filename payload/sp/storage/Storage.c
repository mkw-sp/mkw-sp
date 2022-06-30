#include "Storage.h"

#include "sp/storage/FatStorage.h"
#include "sp/storage/LogFile.h"
#include "sp/storage/NandArcStorage.h"
#include "sp/storage/NetStorage.h"

#include <wchar.h>

// Primary storage: FAT over SD or USB
static Storage fatStorage;
// Secondary storage: NAND archive
static Storage nandArcStorage;
// Secondary storage: LAN NetStorage
static Storage netStorage;

bool Storage_init(void) {
    if (!FatStorage_init(&fatStorage)) {
        return false;
    }

    if (!NandArcStorage_init(&nandArcStorage)) {
        return false;
    }
    fatStorage.next = &nandArcStorage;

    if (NetStorage_init(&netStorage)) {
        nandArcStorage.next = &netStorage;
        netStorage.next = NULL;
    }

    return true;
}

bool Storage_fastOpen(File *file, NodeId id) {
    LOG_FILE_DISABLE();

    assert(file);
    assert(id.storage);

    file->storage = id.storage;
    return id.storage->fastOpen(file, id.id);
}

bool Storage_open(File *file, const wchar_t *path, const char *mode) {
    LOG_FILE_DISABLE();

    assert(file);
    assert(path);

    for (Storage *s = &fatStorage; s != NULL; s = s->next) {
        assert(s->open);
        if (s->open(file, path, mode)) {
            file->storage = s;
            return true;
        }
    }

    file->storage = NULL;
    return false;
}

bool Storage_close(File *file) {
    LOG_FILE_DISABLE();

    assert(file);
    assert(file->storage);

    return file->storage->close(file);
}

bool Storage_read(File *file, void *dst, u32 size, u32 offset) {
    LOG_FILE_DISABLE();

    assert(file);
    assert(dst);
    assert(file->storage);

    return file->storage->read(file, dst, size, offset);
}

bool Storage_write(File *file, const void *src, u32 size, u32 offset) {
    LOG_FILE_DISABLE();

    assert(file);
    assert(src);

    return file->storage->write(file, src, size, offset);
}

bool Storage_sync(File *file) {
    LOG_FILE_DISABLE();

    assert(file);

    return file->storage->sync(file);
}

u64 Storage_size(File *file) {
    LOG_FILE_DISABLE();

    assert(file);
    assert(file->storage);

    return file->storage->size(file);
}

static bool Storage_readFileCommon(File *file, void *dst, u32 size, u32 *readSize) {
    u64 fileSize = Storage_size(file);
    if (fileSize < size) {
        size = fileSize;
    }

    if (!Storage_read(file, dst, size, 0)) {
        Storage_close(file);
        return false;
    }

    if (readSize) {
        *readSize = size;
    }
    return Storage_close(file);
}

bool Storage_fastReadFile(NodeId id, void *dst, u32 size, u32 *readSize) {
    File file;
    if (!Storage_fastOpen(&file, id)) {
        return false;
    }

    return Storage_readFileCommon(&file, dst, size, readSize);
}

bool Storage_readFile(const wchar_t *path, void *dst, u32 size, u32 *readSize) {
    File file;
    if (!Storage_open(&file, path, "r")) {
        return false;
    }

    return Storage_readFileCommon(&file, dst, size, readSize);
}

bool Storage_writeFile(const wchar_t *path, bool overwrite, const void *src, u32 size) {
    File file;
    const char *mode = overwrite ? "w" : "wx";
    if (!Storage_open(&file, path, mode)) {
        return false;
    }

    if (!Storage_write(&file, src, size, 0)) {
        Storage_close(&file);
        return false;
    }

    return Storage_close(&file);
}

// WARNING: Only operates on the primary storage
bool Storage_createDir(const wchar_t *path, bool allowNop) {
    LOG_FILE_DISABLE();

    assert(path);

    return fatStorage.createDir(path, allowNop);
}

bool Storage_fastOpenDir(Dir *dir, NodeId id) {
    LOG_FILE_DISABLE();

    assert(dir);
    assert(id.storage);

    dir->storage = id.storage;
    return id.storage->fastOpenDir(dir, id.id);
}

bool Storage_openDir(Dir *dir, const wchar_t *path) {
    LOG_FILE_DISABLE();

    assert(dir);
    assert(path);

    for (Storage *s = &fatStorage; s != NULL; s = s->next) {
        assert(s->openDir);
        if (s->openDir(dir, path)) {
            dir->storage = s;
            return true;
        }
    }

    dir->storage = NULL;
    return false;
}

bool Storage_readDir(Dir *dir, NodeInfo *info) {
    LOG_FILE_DISABLE();

    assert(dir);
    assert(dir->storage);
    assert(info);

    info->id.storage = dir->storage;
    return dir->storage->readDir(dir, info);
}

bool Storage_closeDir(Dir *dir) {
    LOG_FILE_DISABLE();

    assert(dir);
    assert(dir->storage);

    return dir->storage->closeDir(dir);
}

void Storage_stat(const wchar_t *path, NodeInfo *info) {
    LOG_FILE_DISABLE();

    assert(path);
    assert(info);

    for (Storage *s = &fatStorage; s != NULL; s = s->next) {
        assert(s->stat);
        s->stat(path, info);
        if (info->type != NODE_TYPE_NONE) {
            info->id.storage = s;
            return;
        }
    }
}

// WARNING: Only operates on the primary storage
bool Storage_rename(const wchar_t *srcPath, const wchar_t *dstPath) {
    LOG_FILE_DISABLE();

    assert(srcPath);
    assert(dstPath);

    return fatStorage.rename(srcPath, dstPath);
}

// WARNING: Only operates on the primary storage
bool Storage_remove(const wchar_t *path, bool allowNop) {
    LOG_FILE_DISABLE();

    assert(path);

    return fatStorage.remove(path, allowNop);
}

bool Storage_tryReplace(const wchar_t *path, const void *src, u32 size) {
    LOG_FILE_DISABLE();

    wchar_t newPath[64];
    swprintf(newPath, ARRAY_SIZE(newPath), L"%s.new", path);
    wchar_t oldPath[64];
    swprintf(oldPath, ARRAY_SIZE(oldPath), L"%s.old", path);

    if (!Storage_writeFile(newPath, true, src, size)) {
        SP_LOG("Failed to write to %ls", newPath);
        return false;
    }

    if (!Storage_remove(oldPath, true)) {
        SP_LOG("Failed to remove %ls", oldPath);
        return false;
    }

    if (!Storage_rename(path, oldPath)) {
        SP_LOG("Failed to rename %ls to %ls", path, oldPath);
        // Ignore
    }

    if (!Storage_rename(newPath, path)) {
        SP_LOG("Failed to rename %ls to %ls", newPath, path);
        return false;
    }

    Storage_remove(oldPath, true); // Not a big deal if this fails
    return true;
}
