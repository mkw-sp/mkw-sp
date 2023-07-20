#pragma once

#include "sp/ScopeLock.hh"
#include "sp/net/ProtoSocket.hh"
#include "sp/net/SyncSocket.hh"
#include "sp/storage/Storage.hh"

#include <protobuf/NetStorage.pb.h>

#include <algorithm>
#include <iterator>

namespace SP::Storage {

class NetStorage : public IStorage {
public:
    NetStorage();

    std::optional<FileHandle> fastOpen(u64 id) override;
    std::optional<FileHandle> open(const wchar_t *path, const char *mode) override;

    bool createDir(const wchar_t *path, bool allowNop) override;
    std::optional<DirHandle> fastOpenDir(u64 id) override;
    std::optional<DirHandle> openDir(const wchar_t *path) override;

    std::optional<NodeInfo> stat(const wchar_t *path) override;
    bool rename(const wchar_t *srcPath, const wchar_t *dstPath) override;
    bool remove(const wchar_t *path, bool allowNop) override;

    std::optional<FileHandle> startBenchmark() override;
    void endBenchmark() override;
    u32 getMessageId() override;

private:
    class File : public IFile {
    public:
        std::optional<FileHandle> clone() override;
        bool close() override;
        bool read(void *dst, u32 size, u32 offset) override;
        bool write(const void *src, u32 size, u32 offset) override;
        bool sync() override;
        u64 size() override;

    private:
        NetStorage *m_storage = nullptr;
        std::optional<u32> m_handle{};
        u64 m_size;

        friend class NetStorage;
    };

    class Dir : public IDir, private ARCDir {
    public:
        std::optional<DirHandle> clone() override;
        bool close() override;
        std::optional<NodeInfo> read() override;

    private:
        NetStorage *m_storage = nullptr;
        std::optional<u32> m_handle{};

        friend class NetStorage;
    };

    bool writeFastOpen(u64 id);
    bool writeOpen(const wchar_t *path, const char *mode);
    bool writeClone(u32 handle);
    bool writeClose(u32 handle);
    bool writeRead(u32 handle, u32 size, u64 offset);
    bool writeWrite(u32 handle, u32 size, u64 offset);
    bool writeFastOpenDir(u64 id);
    bool writeOpenDir(const wchar_t *path);
    bool writeCloneDir(u32 handle);
    bool writeCloseDir(u32 handle);
    bool writeReadDir(u32 handle);
    bool writeStat(const wchar_t *path);
    bool writeStartBenchmark();

    std::optional<FileHandle> readOpen(File *file);
    std::optional<DirHandle> readOpenDir(Dir *dir);
    std::optional<NodeInfo> readNodeInfo();
    bool readOk();

    void connect();

    static void *Connect(void *arg);

    template <typename N>
    static N *FindNode(N (&nodes)[32]) {
        return std::find_if(std::begin(nodes), std::end(nodes),
                [](const auto &node) { return !node.m_handle; });
    }

    Mutex m_mutex{};
    u8 m_stack[4096];
    OSThread m_thread;
    std::optional<Net::SyncSocket> m_socketRaw;
    std::optional<Net::ProtoSocket<NetStorageResponse, NetStorageRequest, Net::SyncSocket>>
            m_socket;
    File m_files[32];
    Dir m_dirs[32];

    static const u8 serverPK[hydro_kx_PUBLICKEYBYTES];
};

} // namespace SP::Storage
