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

bool Storage_open(File *file, const char *path, u32 mode) {
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
