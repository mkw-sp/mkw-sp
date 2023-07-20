#include "NetStorage.hh"

#define NET_STORAGE_HOSTNAME "localhost"
#define NET_STORAGE_PORT 21329
#define NET_STORAGE_BLOCK false

namespace SP::Storage {

template <typename T>
bool WarnError(std::expected<T, const wchar_t *> res, const char *name) {
    if (!res) {
        SP_LOG("[Warning] Ignoring %s error: %ls", name, res.error());
        return false;
    } else {
        return true;
    }
}

template <typename T>
void CheckArgs(std::expected<T, const wchar_t *>, const char *) {}

#define TRY_WARN(resExpr, name) \
    ({ \
        auto res = (resExpr); \
        CheckArgs(resExpr, name); \
        if (!res) { \
            SP_LOG("[Warning] Ignoring " name " error: %ls", res.error()); \
            return std::nullopt; \
        } \
        *res; \
    })

NetStorage::NetStorage() {
    for (u32 i = 0; i < std::size(m_files); i++) {
        m_files[i].m_storage = this;
    }
    for (u32 i = 0; i < std::size(m_dirs); i++) {
        m_dirs[i].m_storage = this;
    }

    u8 *stackTop = m_stack + sizeof(m_stack);
    OSCreateThread(&m_thread, Connect, this, stackTop, sizeof(m_stack), 24, 0);
    OSResumeThread(&m_thread);
#if NET_STORAGE_BLOCK
    OSJoinThread(&m_thread, nullptr);
#endif
}

std::optional<FileHandle> NetStorage::fastOpen(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    if (!m_socket) {
        return {};
    }

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (!writeFastOpen(id)) {
        return {};
    }

    return readOpen(file);
}

std::optional<FileHandle> NetStorage::open(const wchar_t *path, const char *mode) {
    ScopeLock<Mutex> lock(m_mutex);

    if (!m_socket) {
        return {};
    }

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (!writeOpen(path, mode)) {
        return {};
    }

    return readOpen(file);
}

bool NetStorage::createDir(const wchar_t * /* path */, bool /* allowNop */) {
    return false;
}

std::optional<DirHandle> NetStorage::fastOpenDir(u64 id) {
    ScopeLock<Mutex> lock(m_mutex);

    if (!m_socket) {
        return {};
    }

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    if (!writeFastOpenDir(id)) {
        return {};
    }

    return readOpenDir(dir);
}

std::optional<DirHandle> NetStorage::openDir(const wchar_t *path) {
    ScopeLock<Mutex> lock(m_mutex);

    if (!m_socket) {
        return {};
    }

    auto *dir = FindNode(m_dirs);
    if (dir == std::end(m_dirs)) {
        return {};
    }

    if (!writeOpenDir(path)) {
        return {};
    }

    return readOpenDir(dir);
}

std::optional<NodeInfo> NetStorage::stat(const wchar_t *path) {
    ScopeLock<Mutex> lock(m_mutex);

    if (!m_socket) {
        return {};
    }

    if (!writeStat(path)) {
        return {};
    }

    return readNodeInfo();
}

bool NetStorage::rename(const wchar_t * /* srcPath */, const wchar_t * /* dstPath */) {
    return false;
}

bool NetStorage::remove(const wchar_t * /* path */, bool /* allowNop */) {
    return false;
}

std::optional<FileHandle> NetStorage::startBenchmark() {
    ScopeLock<Mutex> lock(m_mutex);

    if (!m_socket) {
        return {};
    }

    auto *file = FindNode(m_files);
    if (file == std::end(m_files)) {
        return {};
    }

    if (!writeStartBenchmark()) {
        return {};
    }

    return readOpen(file);
}

void NetStorage::endBenchmark() {}

u32 NetStorage::getMessageId() {
    return 10158;
}

std::optional<FileHandle> NetStorage::File::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!m_storage->m_socket) {
        return {};
    }

    auto *file = FindNode(m_storage->m_files);
    if (file == std::end(m_storage->m_files)) {
        return {};
    }

    if (!m_storage->writeClone(*m_handle)) {
        return {};
    }

    return m_storage->readOpen(file);
}

bool NetStorage::File::close() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!m_storage->m_socket) {
        return {};
    }

    if (!m_storage->writeClose(*m_handle)) {
        return false;
    }

    if (!m_storage->readOk()) {
        return false;
    }

    m_handle.reset();
    return true;
}

bool NetStorage::File::read(void *dst, u32 size, u32 offset) {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!m_storage->m_socket) {
        return false;
    }

    if (!m_storage->writeRead(*m_handle, size, offset)) {
        return false;
    }

    if (!m_storage->readOk()) {
        return false;
    }

    auto *ptr = reinterpret_cast<u8 *>(dst);
    size += offset;
    while (offset < size) {
        u16 chunkSize = std::min(size - offset, static_cast<u32>(0x1000));
        if (!m_storage->m_socketRaw->read(ptr, chunkSize)) {
            return false;
        }
        ptr += chunkSize;
        offset += chunkSize;
    }

    return true;
}

bool NetStorage::File::write(const void *src, u32 size, u32 offset) {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!m_storage->m_socket) {
        return false;
    }

    if (!m_storage->writeWrite(*m_handle, size, offset)) {
        return false;
    }

    auto *ptr = reinterpret_cast<const u8 *>(src);
    size += offset;
    while (offset < size) {
        u16 chunkSize = std::min(size - offset, static_cast<u32>(0x1000));
        if (!m_storage->m_socketRaw->write(ptr, chunkSize)) {
            return false;
        }
        ptr += chunkSize;
        offset += chunkSize;
    }

    return m_storage->readOk();
}

bool NetStorage::File::sync() {
    return true;
}

u64 NetStorage::File::size() {
    return m_size;
}

std::optional<DirHandle> NetStorage::Dir::clone() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!m_storage->m_socket) {
        return {};
    }

    auto *dir = FindNode(m_storage->m_dirs);
    if (dir == std::end(m_storage->m_dirs)) {
        return {};
    }

    if (!m_storage->writeCloneDir(*m_handle)) {
        return {};
    }

    return m_storage->readOpenDir(dir);
}

bool NetStorage::Dir::close() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!m_storage->m_socket) {
        return {};
    }

    if (!m_storage->writeCloseDir(*m_handle)) {
        return false;
    }

    if (!m_storage->readOk()) {
        return false;
    }

    m_handle.reset();
    return true;
}

std::optional<NodeInfo> NetStorage::Dir::read() {
    ScopeLock<Mutex> lock(m_storage->m_mutex);

    if (!m_storage->m_socket) {
        return {};
    }

    if (!m_storage->writeReadDir(*m_handle)) {
        return {};
    }

    return m_storage->readNodeInfo();
}

bool NetStorage::writeFastOpen(u64 id) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_fastOpen_tag;
    request.request.fastOpen.id = id;
    return WarnError(m_socket->writeProto(request), "writeFastOpen");
}

bool NetStorage::writeOpen(const wchar_t *path, const char *mode) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_open_tag;
    snprintf(request.request.open.path, sizeof(request.request.open.path), "%ls", path);
    snprintf(request.request.open.mode, sizeof(request.request.open.mode), "%s", mode);
    return WarnError(m_socket->writeProto(request), "writeOpen");
}

bool NetStorage::writeClone(u32 handle) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_clone_tag;
    request.request.clone.handle = handle;
    return WarnError(m_socket->writeProto(request), "writeClone");
}

bool NetStorage::writeClose(u32 handle) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_close_tag;
    request.request.close.handle = handle;
    return WarnError(m_socket->writeProto(request), "writeClose");
}

bool NetStorage::writeRead(u32 handle, u32 size, u64 offset) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_read_tag;
    request.request.read.handle = handle;
    request.request.read.size = size;
    request.request.read.offset = offset;
    return WarnError(m_socket->writeProto(request), "writeRead");
}

bool NetStorage::writeWrite(u32 handle, u32 size, u64 offset) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_write_tag;
    request.request.write.handle = handle;
    request.request.write.size = size;
    request.request.write.offset = offset;
    return WarnError(m_socket->writeProto(request), "writeWrite");
}

bool NetStorage::writeFastOpenDir(u64 id) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_fastOpenDir_tag;
    request.request.fastOpenDir.id = id;
    return WarnError(m_socket->writeProto(request), "writeFastOpenDir");
}

bool NetStorage::writeOpenDir(const wchar_t *path) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_openDir_tag;
    snprintf(request.request.openDir.path, sizeof(request.request.openDir.path), "%ls", path);
    return WarnError(m_socket->writeProto(request), "writeOpenDir");
}

bool NetStorage::writeCloneDir(u32 handle) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_cloneDir_tag;
    request.request.cloneDir.handle = handle;
    return WarnError(m_socket->writeProto(request), "writeCloneDir");
}

bool NetStorage::writeCloseDir(u32 handle) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_closeDir_tag;
    request.request.closeDir.handle = handle;
    return WarnError(m_socket->writeProto(request), "writeCloseDir");
}

bool NetStorage::writeReadDir(u32 handle) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_readDir_tag;
    request.request.readDir.handle = handle;
    return WarnError(m_socket->writeProto(request), "writeReadDir");
}

bool NetStorage::writeStat(const wchar_t *path) {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_stat_tag;
    snprintf(request.request.stat.path, sizeof(request.request.stat.path), "%ls", path);
    return WarnError(m_socket->writeProto(request), "writeStat");
}

bool NetStorage::writeStartBenchmark() {
    NetStorageRequest request;
    request.which_request = NetStorageRequest_startBenchmark_tag;
    return WarnError(m_socket->writeProto(request), "writeStartBenchmark");
}

std::optional<FileHandle> NetStorage::readOpen(File *file) {
    auto response = TRY_OPT(TRY_WARN(m_socket->readProto(), "readOpen"));
    if (response.which_response != NetStorageResponse_open_tag) {
        SP_LOG("[Warning] Got wrong response for Open request");
        return std::nullopt;
    }

    file->m_handle = response.response.open.handle;
    file->m_size = response.response.open.size;
    return file;
}

std::optional<DirHandle> NetStorage::readOpenDir(Dir *dir) {
    auto response = TRY_OPT(TRY_WARN(m_socket->readProto(), "readOpenDir"));
    if (response.which_response != NetStorageResponse_openDir_tag) {
        SP_LOG("[Warning] Got wrong response for OpenDir request");
        return std::nullopt;
    }

    dir->m_handle = response.response.openDir.handle;
    return dir;
}

std::optional<NodeInfo> NetStorage::readNodeInfo() {
    auto response = TRY_OPT(TRY_WARN(m_socket->readProto(), "readNodeInfo"));
    if (response.which_response != NetStorageResponse_nodeInfo_tag) {
        SP_LOG("[Warning] Got wrong response for readNodeInfo request");
        return std::nullopt;
    }

    NodeInfo info{};
    info.id.storage = this;
    info.id.id = response.response.nodeInfo.id;
    info.type = static_cast<NodeType>(response.response.nodeInfo.type);
    info.size = response.response.nodeInfo.size;
    swprintf(info.name, std::size(info.name), L"%s", response.response.nodeInfo.name);
    return info;
}

bool NetStorage::readOk() {
    auto responseRes = m_socket->readProto();
    if (!responseRes || !(*responseRes)) {
        return false;
    }

    auto response = **responseRes;
    if (response.which_response != NetStorageResponse_ok_tag) {
        return false;
    }

    return true;
}

void NetStorage::connect() {
#if defined(NET_STORAGE_HOSTNAME) && defined(NET_STORAGE_PORT) && defined(NET_STORAGE_PK)
    while (true) {
        Net::SyncSocket socket(NET_STORAGE_HOSTNAME, NET_STORAGE_PORT, serverPK, "storage ");
        if (socket.ok()) {
            m_socketRaw = std::move(socket);
            m_socket = Net::ProtoSocket<NetStorageResponse, NetStorageRequest, Net::SyncSocket>(
                    &*m_socketRaw, NetStorageResponse_fields, NetStorageRequest_fields);
            return;
        }
        OSSleepMilliseconds(1000);
    }
#endif
}

void *NetStorage::Connect(void *arg) {
    reinterpret_cast<NetStorage *>(arg)->connect();
    return nullptr;
}

#ifdef NET_STORAGE_PK
const u8 NetStorage::serverPK[hydro_kx_PUBLICKEYBYTES] = {NET_STORAGE_PK};
#endif

} // namespace SP::Storage
