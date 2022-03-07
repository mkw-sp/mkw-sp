#include "WfsStorage.h"

#include "Bytes.h"

#include <revolution.h>
#include <revolution/ios.h>

#include <stdalign.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

enum {
    MAX_PATH_LENGTH = 0x1fd,
};

enum {
    IOCTL_GET_DEVICE_NAME = 0x5,
    IOCTL_MKDIR = 0xc,
    IOCTL_GLOB_START = 0xd,
    IOCTL_GLOB_NEXT = 0xe,
    IOCTL_GLOB_END = 0xf,
    IOCTL_DELETE = 0x15,
    IOCTL_RENAME = 0x16,
    IOCTL_GET_ATTRIBUTES = 0x17,
    IOCTL_CREATE_OPEN = 0x19,
    IOCTL_OPEN = 0x1a,
    IOCTL_GET_SIZE = 0x1b,
    IOCTL_CLOSE = 0x1e,
    IOCTL_READ_ABSOLUTE = 0x48,
    IOCTL_WRITE_ABSOLUTE = 0x49,
};

enum {
    RESULT_ALREADY_EXISTS = -10027,
    RESULT_NOT_FOUND = -10028,
};

static s32 fd = -1;

static bool WfsStorage_openExisting(File *file, const wchar_t *path, u32 mode) {
    alignas(0x20) u8 in[0x220] = { 0 };
    alignas(0x20) u8 out[0x20] = { 0 };

    write_u32(in, 0x0, mode);
    u32 length = snprintf((char *)in + 0x22, MAX_PATH_LENGTH, "%ls", path);
    if (length > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x20, length);

    if (IOS_Ioctl(fd, IOCTL_OPEN, in, sizeof(in), out, sizeof(out)) < 0) {
        return false;
    }

    file->fd = read_u16(out, 0x14);
    return true;
}

static bool WfsStorage_createOpen(File *file, const wchar_t *path) {
    alignas(0x20) u8 in[0x220] = { 0 };
    alignas(0x20) u8 out[0x20] = { 0 };

    u32 length = snprintf((char *)in + 0x22, MAX_PATH_LENGTH, "%ls", path);
    if (length > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x20, length);

    if (IOS_Ioctl(fd, IOCTL_CREATE_OPEN, in, sizeof(in), out, sizeof(out)) < 0) {
        return false;
    }

    file->fd = read_u16(out, 0x0);
    return true;
}

static bool WfsStorage_delete(const wchar_t *path, bool allowNop);

static bool WfsStorage_openOverwrite(File *file, const wchar_t *path) {
    if (!WfsStorage_delete(path, true)) {
        return false;
    }

    return WfsStorage_createOpen(file, path);
}

static bool WfsStorage_open(File *file, const wchar_t *path, const char *mode) {
    if (!strcmp(mode, "r")) {
        return WfsStorage_openExisting(file, path, 1);
    } else if (!strcmp(mode, "w")) {
        return WfsStorage_openOverwrite(file, path);
    } else if (!strcmp(mode, "wx")) {
        return WfsStorage_createOpen(file, path);
    } else {
        assert(!"Unknown opening mode");
    }
}

static bool WfsStorage_close(File *file) {
    alignas(0x20) u8 in[0x20] = { 0 };

    write_u16(in, 0x4, file->fd);

    return IOS_Ioctl(fd, IOCTL_CLOSE, in, sizeof(in), NULL, 0) >= 0;
}

static bool WfsStorage_read(File *file, void *dst, u32 size, u32 offset) {
    alignas(0x20) u8 in[0x20] = { 0 };

    write_u32(in, 0x0, (u32)dst);
    write_u32(in, 0x4, offset);
    write_u32(in, 0x8, size);
    write_u16(in, 0xc, file->fd);

    s32 result = IOS_Ioctl(fd, IOCTL_READ_ABSOLUTE, in, sizeof(in), NULL, 0);
    if (result < 0 || (u32)result != size) {
        return false;
    }

    ICInvalidateRange(dst, size);
    return true;
}

static bool WfsStorage_write(File *file, const void *src, u32 size, u32 offset) {
    DCFlushRange((void *)src, size);

    alignas(0x20) u8 in[0x20] = { 0 };

    write_u32(in, 0x0, (u32)src);
    write_u32(in, 0x4, offset);
    write_u32(in, 0x8, size);
    write_u16(in, 0xc, file->fd);

    return IOS_Ioctl(fd, IOCTL_WRITE_ABSOLUTE, in, sizeof(in), NULL, 0) >= 0;
}

static bool WfsStorage_sync(File *UNUSED(file)) {
    return true;
}

static u64 WfsStorage_size(File *file) {
    alignas(0x20) u8 in[0x20] = { 0 };
    alignas(0x20) u8 out[0x20] = { 0 };

    write_u16(in, 0x0, file->fd);

    if (IOS_Ioctl(fd, IOCTL_GET_SIZE, in, sizeof(in), out, sizeof(out)) < 0) {
        return 0;
    }

    return read_u32(out, 0x0);
}

static bool WfsStorage_createDir(const wchar_t *path, bool allowNop) {
    alignas(0x20) u8 in[0x220] = { 0 };

    u32 length = snprintf((char *)in + 0x22, MAX_PATH_LENGTH, "%ls", path);
    if (length > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x20, length);

    s32 result = IOS_Ioctl(fd, IOCTL_MKDIR, in, sizeof(in), NULL, 0);
    return result >= 0 || (allowNop && result == RESULT_ALREADY_EXISTS);
}

static bool WfsStorage_nextDirEntry(DirEntry *entry, u8 *out, s32 result) {
    if (result == RESULT_NOT_FOUND) {
        entry->type = NODE_TYPE_NONE;
        return true;
    } else if (result < 0) {
        return false;
    }

    out[0x113] = '\0';
    swprintf(entry->name, sizeof(entry->name) / sizeof(wchar_t), L"%s", (char *)out + 0x14);
    entry->type = read_u32(out, 0x4) & 0x80000000 ? NODE_TYPE_DIR : NODE_TYPE_FILE;

    return true;
}

static bool WfsStorage_openDir(Dir *dir, DirEntry *entry, const wchar_t *path) {
    alignas(0x20) u8 in[0x200] = { 0 };
    alignas(0x20) u8 out[0x120] = { 0 };

    u32 length = snprintf((char *)in + 0x2, MAX_PATH_LENGTH, "%ls/*", path);
    if (length > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x0, length);

    s32 result = IOS_Ioctl(fd, IOCTL_GLOB_START, in, sizeof(in), out, sizeof(out));

    dir->fd = read_u16(out, 0x114);

    return WfsStorage_nextDirEntry(entry, out, result);
}

static bool WfsStorage_readDir(Dir *dir, DirEntry *entry) {
    alignas(0x20) u8 in[0x20] = { 0 };
    alignas(0x20) u8 out[0x120] = { 0 };

    write_u16(in, 0x0, dir->fd);

    s32 result = IOS_Ioctl(fd, IOCTL_GLOB_NEXT, in, sizeof(in), out, sizeof(out));
    if (result == RESULT_NOT_FOUND) {
        entry->type = NODE_TYPE_NONE;
        return true;
    } else if (result < 0) {
        return false;
    }

    return WfsStorage_nextDirEntry(entry, out, result);
}

static bool WfsStorage_closeDir(Dir *dir) {
    alignas(0x20) u8 in[0x20] = { 0 };

    write_u16(in, 0x0, dir->fd);

    return IOS_Ioctl(fd, IOCTL_GLOB_END, in, sizeof(in), NULL, 0) >= 0;
}

static u32 WfsStorage_type(const wchar_t *path) {
    alignas(0x20) u8 in[0x200] = { 0 };
    alignas(0x20) u8 out[0x20] = { 0 };

    u32 length = snprintf((char *)in + 0x2, MAX_PATH_LENGTH, "%ls", path);
    if (length > MAX_PATH_LENGTH) {
        return NODE_TYPE_NONE;
    }
    write_u16(in, 0x0, length);

    if (IOS_Ioctl(fd, IOCTL_GET_ATTRIBUTES, in, sizeof(in), out, sizeof(out)) < 0) {
        return NODE_TYPE_NONE;
    }

    return read_u32(out, 0x4) & 0x80000000 ? NODE_TYPE_DIR : NODE_TYPE_FILE;
}

static bool WfsStorage_rename(const wchar_t *srcPath, const wchar_t *dstPath) {
    alignas(0x20) u8 in[0x400] = { 0 };

    u32 srcLength = snprintf((char *)in + 0x2, MAX_PATH_LENGTH, "%ls", srcPath);
    if (srcLength > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x0, srcLength);

    u32 dstLength = snprintf((char *)in + 0x202, MAX_PATH_LENGTH, "%ls", dstPath);
    if (dstLength > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x200, dstLength);

    return IOS_Ioctl(fd, IOCTL_RENAME, in, sizeof(in), NULL, 0) >= 0;
}

static bool WfsStorage_delete(const wchar_t *path, bool allowNop) {
    alignas(0x20) u8 in[0x200] = { 0 };

    u32 length = snprintf((char *)in + 0x2, MAX_PATH_LENGTH, "%ls", path);
    if (length > MAX_PATH_LENGTH) {
        return NODE_TYPE_NONE;
    }
    write_u16(in, 0x0, length);

    s32 result = IOS_Ioctl(fd, IOCTL_DELETE, in, sizeof(in), NULL, 0);
    return result >= 0 || (allowNop && result == RESULT_NOT_FOUND);
}

bool WfsStorage_init(Storage *storage) {
    assert(fd < 0);

    fd = IOS_Open("/dev/usb/wfssrv", 0);
    if (fd < 0) {
        SP_LOG("[WFS] Failed to open /dev/wfssrv: Returned error %i", fd);
        return false;
    } else {
        SP_LOG("[WFS] Successfully opened interface: ID: %i", fd);
    }

    storage->open = WfsStorage_open;
    storage->close = WfsStorage_close;
    storage->read = WfsStorage_read;
    storage->write = WfsStorage_write;
    storage->sync = WfsStorage_sync;
    storage->size = WfsStorage_size;
    storage->createDir = WfsStorage_createDir;
    storage->openDir = WfsStorage_openDir;
    storage->readDir = WfsStorage_readDir;
    storage->closeDir = WfsStorage_closeDir;
    storage->type = WfsStorage_type;
    storage->rename = WfsStorage_rename;
    storage->delete = WfsStorage_delete;

    return true;
}
