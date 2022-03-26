#include "NetStorageClient.h"
#include <sp/Bytes.h>
#include <sp/ScopeLock.h>
#include <stdio.h>
#include <wchar.h>

enum kNetPacket {
    kNetPacket_Open,
    // kNetPacket_OpenResponse,
    kNetPacket_Read,
    // kNetPacket_ReadResponse,

    kNetPacket_FileRename,
    kNetPacket_FileRemove,

    kNetPacket_DirRead,

    kNetPacket_BulkJsonCmds=11,
};

typedef struct NetRequest {
    // Includes the length of this header
    u32 packet_len;
    // Values of kNetPacket
    u32 packet_type;
    s32 file_id;
} NetRequest;

typedef struct NetRequest_Read {
    struct NetRequest r;
    u32 offset;
    u32 length;
} NetRequest_Read;

typedef struct NetRequest_Open {
    struct NetRequest r;
    wchar_t path[128];
} NetRequest_Open;

typedef struct {
    NetRequest r;
    wchar_t path[128];
} NetRequest_FileRename;

typedef struct {
    s32 file_id;
    u32 file_size;  // 0 if error
} NetRequest_OpenResponse;

typedef struct {
    struct NetRequest r;
} NetRequest_DirRead;

typedef struct {
    u32 type;
    wchar_t path[128];
} NetRequest_DirReadResponse;

typedef struct {
    struct NetRequest r;
    u32 len;
} NetRequest_Json;

static int sIdCounter = 0;
static int AllocID() {
    return sIdCounter++;
}

#define NET_DEBUG(...)
// #define NET_DEBUG SP_LOG

void NetStreamBuf_copy(NetStreamBuf *self, const NetStreamBuf *other) {
    static_assert(sizeof(*self) == sizeof(*other));
    memcpy(self, other, sizeof(*self));
}

bool NetStreamBuf_contains(NetStreamBuf *buf, s32 offset, s32 len) {
    assert(len >= 0);
    assert(offset >= 0);

    if (offset < buf->streamedOffset || offset >= buf->streamedOffset + buf->streamedSize)
        return false;

    if (offset - buf->streamedOffset + len > buf->streamedSize)
        return false;

    return true;
}

void NetStorageClient_create(NetStorageClient *client) {
    memset(client, 0, sizeof(*client));
    TcpSocket_create(&client->sock);
    OSInitMutex(&client->mutex);
}

void NetStorageClient_destroy(NetStorageClient *client) {
    TcpSocket_destroy(&client->sock);
}

bool NetStorageClient_connect(
        NetStorageClient *client, u8 ipa, u8 ipb, u8 ipc, u8 ipd, u32 port) {
    TcpSocket_init(&client->sock);
    return TcpSocket_tryConnectIpv4(&client->sock, ipa, ipb, ipc, ipd, port);
}
void NetStorageClient_disconnect(NetStorageClient *client) {
    assert(client->filesInFlight == 0 && "Files are open");
    TcpSocket_disconnect(&client->sock);
}

static bool NetNode_open(NetNode *node, NetStorageClient *client, const wchar_t *path) {
    assert(node);
    assert(client);
    assert(!node->isOpen && "File/Folder is already open");
    assert(path && *path && "Path is empty");

    SP_SCOPED_MUTEX_LOCK(client->mutex);

    node->client = client;
    node->id = AllocID();

    u32 path_len = sp_wcslen(path);
    assert(path_len < 128);
    NET_DEBUG("[NetFile] Opening id=%u, path=%ls, pathlen=%u", file->node.id, path,
            path_len);

    u32 request_buf_size = sizeof(NetRequest_Open);
    NetRequest_Open req;

    req.r.packet_len = sp_htonl((u32)request_buf_size);
    req.r.packet_type = sp_htonl((u32)kNetPacket_Open);
    req.r.file_id = sp_htonl(node->id);
    memset(req.path, 0, sizeof(req.path));
    memcpy(req.path, path, sizeof(wchar_t) * MIN(path_len, ARRAY_SIZE(req.path) - 1));
    for (size_t i = 0; i < ARRAY_SIZE(req.path); ++i) {
        req.path[i] = sp_htons(req.path[i]);
    }
    if (!TcpSocket_sendBytes(&client->sock, &req, sizeof(req))) {
        SP_LOG("[NetFile] Failed to send Open request");
        return false;
    }

    NetRequest_OpenResponse response;
    if (!TcpSocket_receiveBytes(&client->sock, &response, sizeof(response))) {
        // This is a lot more problematic than above
        SP_LOG("[NetFile] Failed to receieve Open response");
        return false;
    }
    response.file_id = sp_ntohl(response.file_id);
    response.file_size = sp_ntohl(response.file_size);

    NET_DEBUG("[NetFile] -> Opened id=%u,filesize=%u", (unsigned)file->node.id,
            (unsigned)response.file_size);

    if (response.file_size == 0) {
        return false;
    }

    node->isOpen = true;
    node->fileSize = response.file_size;

    return true;
}

bool NetFile_open(NetFile *file, NetStorageClient *client, const wchar_t *path) {
    if (!NetNode_open(&file->node, client, path)) {
        return false;
    }

    file->buffer.streamedOffset = ~0;
    file->buffer.streamedSize = ~0;

    ++client->filesInFlight;
    return true;
}

void NetFile_close(NetFile *file) {
    assert(file->node.isOpen && "File isn't open");

    file->node.isOpen = false;

    assert(file->node.client);
    {
        SP_SCOPED_MUTEX_LOCK(file->node.client->mutex);
        --file->node.client->filesInFlight;
    }
}

bool NetFile_stream(NetFile *file, u32 pos, u32 bytes) {
    assert(bytes <= sizeof(file->buffer));

    SP_SCOPED_MUTEX_LOCK(file->node.client->mutex);

    file->buffer.streamedOffset = pos;
    file->buffer.streamedSize = bytes;

    NET_DEBUG("[NetFile] Streaming id=%u,pos=%u,len=%u", file->node.id, pos, bytes);

    NetRequest_Read req = (NetRequest_Read){
        .r.packet_len = sp_htonl((u32)sizeof(NetRequest_Read)),
        .r.packet_type = sp_htonl((u32)kNetPacket_Read),
        .r.file_id = sp_htonl(file->node.id),
        .offset = sp_htonl(pos),
        .length = sp_htonl(bytes),
    };
    if (!TcpSocket_sendBytes(&file->node.client->sock, &req, sizeof(req))) {
        SP_LOG("[NetFile] Failed to send Read packet");
        return false;
    }

    if (!TcpSocket_receiveBytes(&file->node.client->sock, file->buffer.buffer,
                MIN(sizeof(file->buffer.buffer), bytes))) {
        SP_LOG("[NetFile] Failed to receieve Read response");
        return false;
    }

    // NET_DEBUG("[NetFile] -> Result=%s", file->buffer.buffer);

    return true;
}

u32 NetFile_read(NetFile *file, void *dst, s32 len, s32 offset) {
    assert(file != NULL);
    assert(file->node.isOpen);
    assert(dst != NULL);
    assert(len >= 1);
    assert(offset >= 0);

    NET_DEBUG("[NetFile] Read id=%i,dst=%p,len=%i,offset=%i", (signed)file->node.id, dst,
            (signed)len, (signed)offset);

    for (s32 i = offset; i < offset + len; i += NET_STREAM_CHUNK) {
        // Usually 1024, unless last read
        s32 current_stride = MIN(file->node.fileSize - i, NET_STREAM_CHUNK);
        NET_DEBUG("current_stride=%u", (signed)current_stride);

        // TODO: Double buffer
        // TODO: If seek back 4, this rereads 1024 (uncommon)
        if (NetStreamBuf_contains(&file->buffer, i, current_stride)) {
            // Already cached
        } else {
            // Intentional: Read potentially extra (e.g. if cache miss for first 32B, read
            // the first 1024B)
            if (!NetFile_stream(file, i, current_stride)) {
                return i - offset;
            }
        }
        u32 streambuf_offset = i - file->buffer.streamedOffset;

        NET_DEBUG("COPY to=%p, from=%p, len=%u", (u8 *)dst + i - offset,
                file->buffer.buffer + streambuf_offset, MIN(current_stride, len - i));
        memcpy((u8 *)dst + i - offset, file->buffer.buffer + streambuf_offset,
                MIN(current_stride, offset + len - i));
    }

    return len;
}

bool NetFile_rename(NetFile *UNUSED(file), const wchar_t *UNUSED(path)) {
    return false;
}
bool NetFile_removeAndClose(NetFile *UNUSED(file)) {
    return false;
}

bool NetDir_open(NetDir *dir, NetStorageClient *client, const wchar_t *path) {
    if (!NetNode_open(&dir->node, client, path)) {
        return false;
    }

    return true;
}
bool NetDir_read(NetDir *dir, NetDirEntry *out) {
    SP_SCOPED_MUTEX_LOCK(dir->node.client->mutex);

    NetRequest_DirRead req = (NetRequest_DirRead){
        .r.file_id = sp_htonl((s32)dir->node.id),
        .r.packet_type = sp_htonl((u32)kNetPacket_DirRead),
        .r.packet_len = sp_htonl((u32)sizeof(NetRequest_DirRead)),
    };

    TcpSocket *sock = &dir->node.client->sock;

    if (!TcpSocket_sendBytes(sock, &req, sizeof(req))) {
        return false;
    }

    NetRequest_DirReadResponse response;
    if (!TcpSocket_receiveBytes(sock, &response, sizeof(response))) {
        return false;
    }
    response.type = sp_ntohl(response.type);

    if (response.type == 0) {
        return false;
    }

    for (size_t i = 0; i < ARRAY_SIZE(response.path); ++i) {
        response.path[i] = sp_ntohs(response.path[i]);
    }

    if (out != NULL) {
        memcpy(out->name, response.path, MIN(sizeof(out->name), sizeof(response.path)));
        out->name[ARRAY_SIZE(out->name) - 1] = L'\0';
        out->isDir = response.type == 2;
    }

    return true;
}
void NetDir_close(NetDir *UNUSED(dir)) {
    // Do nothing
}

bool NetFile_write(NetFile *UNUSED(file), const void *UNUSED(src), u32 UNUSED(size),
        u32 UNUSED(offset)) {
    return false;
}

bool NetStorage_sendJSONCommands(NetStorageClient *self, const char *str) {
    SP_SCOPED_MUTEX_LOCK(self->mutex);

    const u32 len = strlen(str);

    NetRequest_Json req = (NetRequest_Json){
        .r.file_id = sp_htonl((s32)-1),
        .r.packet_type = sp_htonl((u32)kNetPacket_BulkJsonCmds),
        .r.packet_len = sp_htonl((u32)sizeof(NetRequest_Json)),
        .len = sp_htonl(len),
    };

    if (!TcpSocket_sendBytes(&self->sock, &req, sizeof(req))) {
        return false;
    }
    if (!TcpSocket_sendBytes(&self->sock, str, len)) {
        return false;
    }

    return true;
}
bool NetStorage_sendJSONCommandsUTF16(NetStorageClient *self, const wchar_t *str) {
    SP_SCOPED_MUTEX_LOCK(self->mutex);

    const u32 len = wcslen(str);

    NetRequest_Json req = (NetRequest_Json){
        .r.file_id = sp_htonl((s32)-1),
        .r.packet_type = sp_htonl((u32)kNetPacket_BulkJsonCmds),
        .r.packet_len = sp_htonl((u32)sizeof(NetRequest_Json)),
        .len = sp_htonl((u32)(0x80000000 | (len * sizeof(wchar_t)))),
    };

    if (!TcpSocket_sendBytes(&self->sock, &req, sizeof(req))) {
        return false;
    }
    if (!TcpSocket_sendBytes(&self->sock, str, len * sizeof(wchar_t))) {
        return false;
    }

    return true;
}
