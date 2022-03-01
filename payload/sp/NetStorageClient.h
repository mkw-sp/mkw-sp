#pragma once

#include <Common.h>
#include <sp/Tcp.h>
#include <string.h>

//
// Stream buffer
//

enum {
    NET_STREAM_CHUNK = 1024,
};

typedef struct {
    s32 streamedOffset;
    s32 streamedSize;
    u8 buffer[NET_STREAM_CHUNK];
} NetStreamBuf;

void NetStreamBuf_copy(NetStreamBuf *self, const NetStreamBuf *other);
bool NetStreamBuf_contains(NetStreamBuf *buf, s32 offset, s32 len);

//
// Storage client
//

typedef struct {
    u32 filesInFlight;
    TcpSocket sock;
    OSMutex mutex;
} NetStorageClient;

void NetStorageClient_create(NetStorageClient *client);
void NetStorageClient_destroy(NetStorageClient *client);

bool NetStorageClient_connect(
        NetStorageClient *client, u8 ipa, u8 ipb, u8 ipc, u8 ipd, u32 port);
void NetStorageClient_disconnect(NetStorageClient *client);

//
// Net file
//

typedef struct {  // Zero initialized
    NetStorageClient *client;
    s32 fileSize;
    u32 isOpen;
    s32 id;
    NetStreamBuf buffer;
} NetFile;

static inline void NetFile_create(NetFile *file) {
    memset(file, 0, sizeof(*file));
}
bool NetFile_open(NetFile *file, NetStorageClient *client, const wchar_t *path);
void NetFile_close(NetFile *file);
u32 NetFile_read(NetFile *file, void *dst, s32 len, s32 offset);
// Prefetch
bool NetFile_stream(NetFile *file, u32 pos, u32 bytes);
