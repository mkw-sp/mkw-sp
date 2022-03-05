#include "NetStorage.h"
#include "NetStorageClient.h"
#include "ScopeLock.h"

#include <stdio.h>
#include <wchar.h>

NetStorageClient sNetStorageClient;
static bool sNetStorageConnected;

NetFile sNetFiles[12];
u32 sOpenNetFiles;
s32 sNumOpenNetFiles;

static OSMutex sNetMutex;

static void MarkFdUsed(s32 fd, bool used) {
    sOpenNetFiles &= ~(1 << fd);
    sOpenNetFiles |= (used << fd);
}
static bool IsFdFree(s32 fd) {
    return (sOpenNetFiles & (1 << fd)) == 0;
}
static s32 GetFreeFd() {
    for (u32 i = 0; i < ARRAY_SIZE(sNetFiles); ++i) {
        if (IsFdFree(i)) {
            return i;
        }
    }

    return -1;
}

static NetFile *GetNetFileByFd(s32 fd) {
    if (fd >= (s32)ARRAY_SIZE(sNetFiles)) {
        return NULL;
    }

    return &sNetFiles[fd];
}

// WARNING: File creation not supported. NetFile is read-only.
static bool NetStorage_open(File *file, const wchar_t *path, const char *UNUSED(mode)) {
    if (!sNetStorageConnected) {
        return false;
    }

    if (sNumOpenNetFiles == ARRAY_SIZE(sNetFiles)) {
        return false;
    }

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    s32 fd = GetFreeFd();
    if (fd < 0) {
        return false;
    }

    NetFile *netFile = GetNetFileByFd(fd);
    if (NetFile_open(netFile, &sNetStorageClient, path)) {
        ++sNumOpenNetFiles;
        MarkFdUsed(fd, true);
        file->fd = fd;
        return true;
    }

    memset(&netFile, 0, sizeof(netFile));
    return false;
}

static bool NetStorage_close(File *file) {
    NetFile *netFile = GetNetFileByFd(file->fd);
    assert(netFile);

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    NetFile_close(netFile);
    memset(&netFile, 0, sizeof(netFile));

    MarkFdUsed(file->fd, false);
    --sNumOpenNetFiles;
    assert(sNumOpenNetFiles >= 0);

    return true;
}
static bool NetStorage_read(File *file, void *dst, u32 size, u32 offset) {
    NetFile *netFile = GetNetFileByFd(file->fd);
    assert(netFile);

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    const u32 amount_read = NetFile_read(netFile, dst, size, offset);

    return amount_read == size;
}
static bool NetStorage_write(File *UNUSED(file), const void *UNUSED(src),
        u32 UNUSED(size), u32 UNUSED(offset)) {
    return false;
}
static bool NetStorage_sync(File *UNUSED(file)) {
    return false;
}
static u64 NetStorage_size(File *file) {
    NetFile *netFile = GetNetFileByFd(file->fd);
    assert(netFile);

    return netFile->fileSize;
}
static bool NetStorage_createDir(const wchar_t *UNUSED(path), bool UNUSED(allowNop)) {
    return false;
}
static bool NetStorage_openDir(Dir *UNUSED(dir), const wchar_t *path) {
    SP_LOG("Open dir %ls", path);
    return false;
}
static bool NetStorage_readDir(Dir *UNUSED(dir), DirEntry *UNUSED(entry)) {
    return false;
}
static bool NetStorage_closeDir(Dir *UNUSED(dir)) {
    return false;
}
static u32 NetStorage_type(const wchar_t *path) {
    // SP_LOG("Type: %ls\n", path);

    if (!sNetStorageConnected) {
        return NODE_TYPE_NONE;
    }

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    NetFile f;
    memset(&f, 0, sizeof(f));
    if (NetFile_open(&f, &sNetStorageClient, path)) {
        NetFile_close(&f);
        return NODE_TYPE_FILE;
    }

    return NODE_TYPE_NONE;
}
static bool NetStorage_rename(
        const wchar_t *UNUSED(srcPath), const wchar_t *UNUSED(dstPath)) {
    return false;
}
static bool NetStorage_delete(const wchar_t *UNUSED(path), bool UNUSED(allowNop)) {
    return false;
}

// For now, NetStorage is always active, although it may be connected/disconected at a
// later date.
bool NetStorage_init(Storage *storage) {
    NetStorageClient_create(&sNetStorageClient);
    // FIXME: atexit for NetStorageClient_destroy

    OSInitMutex(&sNetMutex);

#ifdef STARTUP_NETSTORAGE_IP
    SP_LOG("========================");
    SP_LOG("Starting SOCKET library");
    {
        int res = SOStartup();
        assert((res == 0 || res == -7) && "Failed to SOStartup");
    }

    SP_LOG("Connecting to " STARTUP_NETSTORAGE_IP "...");
    {
        int ipv4[5];
        sscanf(STARTUP_NETSTORAGE_IP, "%d.%d.%d.%d:%d", ipv4, ipv4 + 1, ipv4 + 2,
                ipv4 + 3, ipv4 + 4);
        bool connected = NetStorage_connect(ipv4[0], ipv4[1], ipv4[2], ipv4[3], ipv4[4]);

        assert(connected && "Failed to connect to " STARTUP_NETSTORAGE_IP);
    }
    SP_LOG("..connected");
    SP_LOG("========================");
#endif  // STARTUP_NETSTORAGE_IP

    storage->open = NetStorage_open;
    storage->close = NetStorage_close;
    storage->read = NetStorage_read;
    storage->write = NetStorage_write;
    storage->sync = NetStorage_sync;
    storage->size = NetStorage_size;
    storage->createDir = NetStorage_createDir;
    storage->openDir = NetStorage_openDir;
    storage->readDir = NetStorage_readDir;
    storage->closeDir = NetStorage_closeDir;
    storage->type = NetStorage_type;
    storage->rename = NetStorage_rename;
    storage->delete = NetStorage_delete;

    return true;
}

bool NetStorage_connect(u8 ipa, u8 ipb, u8 ipc, u8 ipd, u32 port) {
    assert(!sNetStorageConnected && "Network storage is already connected");

    sNetStorageConnected =
            NetStorageClient_connect(&sNetStorageClient, ipa, ipb, ipc, ipd, port);
    return sNetStorageConnected;
}
void NetStorage_disconnect(void) {
    if (sNetStorageConnected) {
        NetStorageClient_disconnect(&sNetStorageClient);
        sNetStorageConnected = false;
    }
}
