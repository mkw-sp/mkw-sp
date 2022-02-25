#include "Storage.h"

#include "FatStorage.h"

static Storage storage;

static bool Storage_find(void) {
    if (FatStorage_init(&storage)) {
        return true;
    }

    return false;
}

bool Storage_init(void) {
    return Storage_find();
}

bool Storage_open(File *file, const wchar_t *path, u32 mode) {
    assert(file);
    assert(path);

    return storage.open(file, path, mode);
}

bool Storage_close(File *file) {
    assert(file);

    return storage.close(file);
}

bool Storage_read(File *file, void *dst, u32 size, u32 *readSize) {
    assert(file);
    assert(dst);
    assert(readSize);

    return storage.read(file, dst, size, readSize);
}

bool Storage_write(File *file, const void *src, u32 size, u32 *writtenSize) {
    assert(file);
    assert(src);
    assert(writtenSize);

    return storage.write(file, src, size, writtenSize);
}

u64 Storage_size(File *file) {
    return storage.size(file);
}

bool Storage_lseek(File *file, u64 offset) {
    assert(file);

    if (!storage.lseek(file, offset)) {
        return false;
    }

    return storage.tell(file) == offset;
}

u64 Storage_tell(File *file) {
    assert(file);

    return storage.tell(file);
}

bool Storage_readFile(const wchar_t *path, void *dst, u32 size, u32 *readSize) {
    File file;
    if (!Storage_open(&file, path, MODE_READ)) {
        return false;
    }

    u64 fileSize = Storage_size(&file);
    if (fileSize < size) {
        size = fileSize;
    }

    if (!Storage_read(&file, dst, size, readSize)) {
        Storage_close(&file);
        return false;
    }

    return Storage_close(&file);
}

bool Storage_writeFile(const wchar_t *path, bool overwrite, const void *src, u32 size) {
    File file;
    u32 mode = MODE_WRITE;
    if (overwrite) {
        mode |= MODE_CREATE_ALWAYS;
    } else {
        mode |= MODE_CREATE_NEW;
    }
    if (!Storage_open(&file, path, mode)) {
        return false;
    }

    u32 writtenSize;
    if (!Storage_write(&file, src, size, &writtenSize)) {
        Storage_close(&file);
        return false;
    }
    if (writtenSize != size) {
        Storage_close(&file);
        return false;
    }

    return Storage_close(&file);
}

bool Storage_createDir(const wchar_t *path, bool overwrite) {
    assert(path);

    return storage.createDir(path, overwrite);
}

bool Storage_openDir(Dir *dir, const wchar_t *path) {
    assert(dir);
    assert(path);

    return storage.openDir(dir, path);
}

bool Storage_readDir(Dir *dir, DirEntry *entry) {
    assert(dir);
    assert(entry);

    return storage.readDir(dir, entry);
}

bool Storage_closeDir(Dir *dir) {
    assert(dir);

    return storage.closeDir(dir);
}
