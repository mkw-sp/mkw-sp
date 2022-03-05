#include "WfsStorage.h"

#include "Bytes.h"

#include <revolution.h>
#include <revolution/ios.h>

#include <stdalign.h>
#include <stdio.h>
#include <string.h>

enum {
    MAX_PATH_LENGTH = 0x1fd,
};

enum {
    IOCTL_GET_DEVICE_NAME = 0x5,
    IOCTL_RENAME = 0x16,
    IOCTL_GET_ATTRIBUTES = 0x17,
    IOCTL_CREATE_OPEN = 0x19,
    IOCTL_OPEN = 0x1a,
    IOCTL_GET_SIZE = 0x1b,
    IOCTL_CLOSE = 0x1e,
    IOCTL_READ_ABSOLUTE = 0x48,
    IOCTL_WRITE_ABSOLUTE = 0x49,
};

static s32 fd = -1;
static char deviceName[0x20];

static bool WfsStorage_getDeviceName(void) {
    alignas(0x20) u32 deviceId = 0;
    alignas(0x20) u8 out[0x20];

    if (IOS_Ioctl(fd, IOCTL_GET_DEVICE_NAME, &deviceId, sizeof(deviceId), &out, sizeof(out)) < 0) {
        SP_LOG("[WFS] Failed to get device name");
        return false;
    }

    u32 length = out[0];
    if (length >= 0x20) {
        SP_LOG("[WFS] Got invalid device name");
        return false;
    }

    memcpy(deviceName, out + 1, length);
    deviceName[length] = '\0';
    SP_LOG("[WFS] Successfully got device name %s", deviceName);
    return true;
}

static bool WfsStorage_openExisting(File *file, const wchar_t *path, u32 mode) {
    alignas(0x20) u8 in[0x220] = { 0 };
    alignas(0x20) u8 out[0x20] = { 0 };

    write_u32(in, 0x0, mode);
    u32 length = snprintf((char *)in + 0x22, MAX_PATH_LENGTH, "/vol/%s%ls", deviceName, path);
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

static bool WfsStorage_createOpen(File *file, const wchar_t *path, u32 mode) {
    alignas(0x20) u8 in[0x220] = { 0 };
    alignas(0x20) u8 out[0x20] = { 0 };

    write_u32(in, 0x0, mode);
    u32 length = snprintf((char *)in + 0x22, MAX_PATH_LENGTH, "/vol/%s%ls", deviceName, path);
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

static bool WfsStorage_openOverwrite(File *file, const wchar_t *path, u32 mode) {
    if (!WfsStorage_delete(path, true)) {
        return false;
    }

    return WfsStorage_createOpen(file, path, mode);
}

static bool WfsStorage_close(File *file);

static bool WfsStorage_openNewOnly(File *file, const wchar_t *path, u32 mode) {
    if (WfsStorage_openExisting(file, path, 1)) {
        WfsStorage_close(file);
        return false;
    }

    return WfsStorage_createOpen(file, path, mode);
}

static bool WfsStorage_open(File *file, const wchar_t *path, const char *mode) {
    if (!strcmp(mode, "r")) {
        return WfsStorage_openExisting(file, path, 1);
    } else if (!strcmp(mode, "w")) {
        return WfsStorage_openOverwrite(file, path, 2);
    } else if (!strcmp(mode, "wx")) {
        return WfsStorage_openNewOnly(file, path, 2);
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

    s32 result = IOS_Ioctl(fd, IOCTL_WRITE_ABSOLUTE, in, sizeof(in), NULL, 0);
    if (result < 0 || (u32)result != size) {
        return false;
    }

    return true;
}

static bool WfsStorage_sync(File *file) {
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
    // TODO implement
    return true;
}

static bool WfsStorage_openDir(Dir *dir, const wchar_t *path) {
    // TODO implement
    return true;
}

static bool WfsStorage_readDir(Dir *dir, DirEntry *entry) {
    // TODO implement
    entry->type = NODE_TYPE_NONE;
    return true;
}

static bool WfsStorage_closeDir(Dir *dir) {
    // TODO implement
    return true;
}

static u32 WfsStorage_type(const wchar_t *path) {
    alignas(0x20) u8 in[0x200] = { 0 };
    alignas(0x20) u8 out[0x20] = { 0 };

    u32 length = snprintf((char *)in + 0x2, MAX_PATH_LENGTH, "/vol/%s%ls", deviceName, path);
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

    char *srcBuf = (char *)in + 0x2;
    u32 srcLength = snprintf(srcBuf, MAX_PATH_LENGTH, "/vol/%s%ls", deviceName, srcPath);
    if (srcLength > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x0, srcLength);

    char *dstBuf = (char *)in + 0x202;
    u32 dstLength = snprintf(dstBuf, MAX_PATH_LENGTH, "/vol/%s%ls", deviceName, dstPath);
    if (dstLength > MAX_PATH_LENGTH) {
        return false;
    }
    write_u16(in, 0x200, dstLength);

    return IOS_Ioctl(fd, IOCTL_RENAME, in, sizeof(in), NULL, 0) >= 0;
}

static bool WfsStorage_delete(const wchar_t *path, bool allowNop) {
    // TODO implement
    return true;
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

    if (!WfsStorage_getDeviceName()) {
        return false;
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

    return false; // Not fully supported yet
}
