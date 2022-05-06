#include "Fs.h"

#include <common/Cache.h>
#include <common/Ios.h>

#include <stdalign.h>
#include <string.h>

typedef void (*LoaderEntryFunc)(void);

#if 0
static alignas(0x20) const char loaderTmpPath[] = "/tmp/loader.bin";
#endif
static alignas(0x20) const char assetsTmpPath[] = "/tmp/assets.arc";

extern const u8 loader[];
extern const u32 loaderSize;
extern const u8 assets[];
extern const u32 assetsSize;

#if 0
static bool readFile(const char *path, void *dst, u32 size, u32 *fileSize) {
    s32 fd = Ios_open(path, MODE_READ);
    if (fd < 0) {
        Ios_close(fd);
        return false;
    }

    FsFileStats fileStats;
    if (!Fs_getFileStats(fd, &fileStats)) {
        Ios_close(fd);
        return false;
    }
    if (fileStats.size < size) {
        size = fileStats.size;
    }

    s32 result = Ios_read(fd, dst, size);
    if (result < 0 || (u32)result != size) {
        Ios_close(fd);
        return false;
    }

    if (Ios_close(fd) != 0) {
        return false;
    }

    if (fileSize) {
        *fileSize = fileStats.size;
    }
    return true;
}
#endif

static bool writeFile(const char *path, const void *src, u32 size) {
    s32 fd = Ios_open(path, MODE_WRITE);
    if (fd < 0) {
        if (!Fs_createFile(path, 0, FS_PERM_READ | FS_PERM_WRITE, 0, 0)) {
            return false;
        }
        fd = Ios_open(path, MODE_WRITE);
        if (fd < 0) {
            return false;
        }
    }

    s32 result = Ios_write(fd, src, size);
    if (result < 0 || (u32)result != size) {
        Ios_close(fd);
        return false;
    }

    return Ios_close(fd) == 0;
}

void Stub_run(void) {
    Ios_init();

    if (!Fs_init()) {
        return;
    }

    if (!writeFile(assetsTmpPath, assets, assetsSize)) {
        return;
    }

    if (!Fs_deinit()) {
        return;
    }

    memcpy((void *)0x80b00000, loader, loaderSize);
    ICInvalidateRange((void *)0x80b00000, loaderSize);

    LoaderEntryFunc loaderEntry = (LoaderEntryFunc)0x80b00000;
    loaderEntry();
}
