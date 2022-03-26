#include "NetStorage.h"
#include "NetStorageClient.h"
#include "ScopeLock.h"

#include <stdio.h>
#include <wchar.h>

static NetStorageClient sNetStorageClient;
static bool sNetStorageConnected;

static NetFile sNetFiles[12];
static u32 sOpenNetFiles;
static s32 sNumOpenNetFiles;

static NetDir sNetDirs[12];

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
    if (fd < 0 || fd >= (s32)ARRAY_SIZE(sNetFiles)) {
        return NULL;
    }

    return &sNetFiles[fd];
}

static bool NetStorage_fastOpen(File *UNUSED(file), u64 UNUSED(id)) {
    return false;
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
static bool NetStorage_write(File *file, const void *src,
        u32 size, u32 offset) {
    NetFile *netFile = GetNetFileByFd(file->fd);
    assert(netFile);

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    return NetFile_write(netFile, src, size, offset);
}
static bool NetStorage_sync(File *UNUSED(file)) {
    return false;
}
static u64 NetStorage_size(File *file) {
    NetFile *netFile = GetNetFileByFd(file->fd);
    assert(netFile);

    return netFile->node.fileSize;
}
static bool NetStorage_createDir(const wchar_t *UNUSED(path), bool UNUSED(allowNop)) {
    return false;
}
static bool NetStorage_fastOpenDir(Dir *UNUSED(dir), u64 UNUSED(id)) {
    return false;
}
static bool NetStorage_openDir(Dir *dir, const wchar_t *path) {
    SP_LOG("Open dir %ls", path);

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    NetDir *it = NULL;
    for (size_t i = 0; i < ARRAY_SIZE(sNetDirs); ++i) {
        if (!sNetDirs[i].node.isOpen) {
            it = &sNetDirs[i];
        }
    }

    if (it == NULL) {
        return false;
    }

    if (!NetDir_open(it, &sNetStorageClient, path)) {
        return false;   
    }

    dir->fd = it - &sNetDirs[0];
    return true;
}
static bool NetStorage_readDir(Dir *dir, NodeInfo *info) {
    assert(dir != NULL);
    assert(dir->fd < ARRAY_SIZE(sNetDirs));
    assert(sNetDirs[dir->fd].node.isOpen);

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    NetDirEntry tmp;
    if (!NetDir_read(&sNetDirs[dir->fd], &tmp)) {
        return false;
    }

    if (info != NULL) {
        memcpy(info->name, &tmp.name, MIN(sizeof(info->name), sizeof(tmp.name)));
        info->name[ARRAY_SIZE(info->name) - 1] = L'\0';
        info->type = tmp.isDir ? NODE_TYPE_DIR : NODE_TYPE_FILE;
    }

    return true;
}
static bool NetStorage_closeDir(Dir *dir) {
    assert(dir != NULL);
    assert(dir->fd < ARRAY_SIZE(sNetDirs));
    assert(sNetDirs[dir->fd].node.isOpen);

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    NetDir_close(&sNetDirs[dir->fd]);
    memset(&sNetDirs[dir->fd], 0, sizeof(sNetDirs[dir->fd]));
    
    return true;
}
static void NetStorage_stat(const wchar_t *path, NodeInfo *info) {
    // SP_LOG("Type: %ls\n", path);

    info->type = NODE_TYPE_NONE;

    if (!sNetStorageConnected) {
        return;
    }

    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    {
        NetFile f;
        memset(&f, 0, sizeof(f));
        if (NetFile_open(&f, &sNetStorageClient, path)) {
            NetFile_close(&f);
            wchar_t *name = wcsrchr(path, '/');
            assert(name);
            name++;
            u32 length = wcslen(name);
            assert(length + 1 < ARRAY_SIZE(info->name));
            wmemcpy(info->name, name, length);
            info->type = NODE_TYPE_FILE;
            return;
        }
    }

    {
        NetDir d;
        memset(&d, 0, sizeof(d));

        if (NetDir_open(&d, &sNetStorageClient, path)) {
            NetDir_close(&d);
            wchar_t *name = wcsrchr(path, '/');
            assert(name);
            name++;
            u32 length = wcslen(name);
            assert(length + 1 < ARRAY_SIZE(info->name));
            wmemcpy(info->name, name, length);
            info->type = NODE_TYPE_FILE;
            return;
        }
    }
}
static bool NetStorage_rename(
        const wchar_t *srcPath, const wchar_t *dstPath) {
    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    NetFile f;
    memset(&f, 0, sizeof(f));
    if (!NetFile_open(&f, &sNetStorageClient, srcPath)) {
        return false;
    }

    const bool result = NetFile_rename(&f, dstPath);
    NetFile_close(&f);

    return result;
}
static bool NetStorage_remove(const wchar_t *path, bool UNUSED(allowNop)) {
    SP_SCOPED_MUTEX_LOCK(sNetMutex);

    NetFile f;
    memset(&f, 0, sizeof(f));
    if (!NetFile_open(&f, &sNetStorageClient, path)) {
        return false;
    }

    return NetFile_removeAndClose(&f);
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

    storage->fastOpen = NetStorage_fastOpen;
    storage->open = NetStorage_open;
    storage->close = NetStorage_close;
    storage->read = NetStorage_read;
    storage->write = NetStorage_write;
    storage->sync = NetStorage_sync;
    storage->size = NetStorage_size;
    storage->createDir = NetStorage_createDir;
    storage->fastOpenDir = NetStorage_fastOpenDir;
    storage->openDir = NetStorage_openDir;
    storage->readDir = NetStorage_readDir;
    storage->closeDir = NetStorage_closeDir;
    storage->stat = NetStorage_stat;
    storage->rename = NetStorage_rename;
    storage->remove = NetStorage_remove;

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
