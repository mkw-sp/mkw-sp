#include "NetStorageClient.h"
#include <sp/ScopeLock.h>

enum kNetPacket {
    kNetPacket_Open,
    // kNetPacket_OpenResponse,
    kNetPacket_Read,
    // kNetPacket_ReadResponse,
};

typedef struct {
    // Includes the length of this header
    u32 packet_len;
    // Values of kNetPacket
    u32 packet_type;
    s32 file_id;
} NetRequest;

typedef struct {
    NetRequest;
    u32 offset;
    u32 length;
} NetRequest_Read;

typedef struct {
    NetRequest;
    char path[64];
} NetRequest_Open;

typedef struct {
    s32 file_id;
    u32 file_size;  // 0 if error
} NetRequest_OpenResponse;

static int sIdCounter = 0;
static int AllocID() {
    return sIdCounter++;
}

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
    TcpSocket_init(&client->sock);
}

void NetStorageClient_destroy(NetStorageClient *client) {
    TcpSocket_destroy(&client->sock);
}

bool NetStorageClient_connect(
        NetStorageClient *client, u8 ipa, u8 ipb, u8 ipc, u8 ipd, u32 port) {
    return TcpSocket_tryConnectIpv4(&client->sock, ipa, ipb, ipc, ipd, port);
}
void NetStorageClient_disconnect(NetStorageClient *client) {
    assert(client->filesInFlight == 0 && "Files are open");
    TcpSocket_disconnect(&client->sock);
}

bool NetFile_open(NetFile *file, NetStorageClient *client, const char *path) {
    assert(file);
    assert(client);
    assert(!file->isOpen && "File is already open");
    assert(path && *path && "Path is empty");

    SP_SCOPED_MUTEX_LOCK(client->mutex);

    file->client = client;
    file->id = AllocID();

    u32 path_len = strlen(path);
    assert(path_len < 64);
    SP_LOG("[NetFile] Opening id=%u,path=%s, pathlen=%u", file->id, path, path_len);

    u8 request_buf_size = sizeof(NetRequest_Open);
    NetRequest_Open req;

    req.packet_len = request_buf_size;
    req.packet_type = kNetPacket_Open;
    req.file_id = file->id;
    memset(req.path, 0, sizeof(req.path));
    memcpy(req.path, path, MIN(path_len, sizeof(req.path) - 1));
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

    SP_LOG("[NetFile] -> Opened id=%u,filesize=%u", file->id, response.file_size);

    if (response.file_size == 0) {
        return false;
    }

    file->isOpen = true;
    file->fileSize = response.file_size;
    file->buffer.streamedOffset = ~0;
    file->buffer.streamedSize = ~0;

    ++client->filesInFlight;
    return true;
}

void NetFile_close(NetFile *file) {
    assert(file->isOpen && "File isn't open");

    file->isOpen = false;
    --file->client->filesInFlight;
}

bool NetFile_stream(NetFile *file, u32 pos, u32 bytes) {
    assert(bytes <= sizeof(file->buffer));

    SP_SCOPED_MUTEX_LOCK(file->client->mutex);

    file->buffer.streamedOffset = pos;
    file->buffer.streamedSize = bytes;

    SP_LOG("[NetFile] Streaming id=%u,pos=%u,len=%u", file->id, pos, bytes);

    NetRequest_Read req = (NetRequest_Read){
        .packet_len = sizeof(NetRequest_Read),
        .packet_type = kNetPacket_Read,
        .file_id = file->id,
        .offset = pos,
        .length = bytes,
    };
    if (!TcpSocket_sendBytes(&file->client->sock, &req, sizeof(req))) {
        SP_LOG("[NetFile] Failed to send Read packet");
        return false;
    }

    if (!TcpSocket_receiveBytes(&file->client->sock, file->buffer.buffer,
                MIN(sizeof(file->buffer.buffer), bytes))) {
        SP_LOG("[NetFile] Failed to receieve Read response");
        return false;
    }

    SP_LOG("[NetFile] -> Result=%s", file->buffer.buffer);

    return true;
}

u32 NetFile_read(NetFile *file, void *dst, s32 len, s32 offset) {
    assert(file != NULL);
    assert(file->isOpen);
    assert(dst != NULL);
    assert(len >= 32);
    assert(offset >= 0);

    SP_LOG("[NetFile] Read id=%i,dst=%p,len=%i,offset=%i", (signed)file->id, dst,
            (signed)len, (signed)offset);

    for (s32 i = offset; i < len; i += 1024) {
        // Usually 1024, unless last read
        s32 current_stride = MIN(file->fileSize - i, 1024);
        SP_LOG("current_stride=%u", (signed)current_stride);

        // TODO: Double buffer
        // TODO: If seek back 4, this rereads 1024 (uncommon)
        if (NetStreamBuf_contains(&file->buffer, i, current_stride)) {
            // Already cached
        } else {
            // Intentional: Read potentially extra (e.g. if cache miss for first 32B, read
            // the first 1024B)
            if (!NetFile_stream(file, i, current_stride)) {
                return i;
            }
        }
        u32 streambuf_offset = i - file->buffer.streamedOffset;
        memcpy((u8 *)dst + i, file->buffer.buffer + streambuf_offset,
                MIN(current_stride, len - i));
    }

    return len;
}
