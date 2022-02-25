#include "Tcp.h"

// Spammy logs
#ifdef TCP_DEBUG
#define TCP_VERBOSE_LOG OSReport
#else
#define TCP_VERBOSE_LOG(...)
#endif

// Fallible APIs will crash in this file
#ifdef TCP_DEBUG
#define TCP_EARLY_ASSERT assert
#else
#define TCP_EARLY_ASSERT(v)
#endif

void TcpSocket_create(TcpSocket *sock) {
    sock->handle = -1;
    sock->isConnected = false;
}

void TcpSocket_destroy(TcpSocket *sock) {
    if (sock->handle >= 0 && sock->isConnected)
        TcpSocket_disconnect(sock);
}

void TcpSocket_init(TcpSocket *sock) {
    assert(!sock->isConnected && sock->handle < 0 && "TcpSocket is already initialized.");
    sock->handle = SOSocket(SO_PF_INET, SO_SOCK_STREAM, 0);
    TCP_VERBOSE_LOG("[TCP] Created with ID %i\n", sock->handle);
    assert(sock->handle >= 0);
    sock->isConnected = false;
}

bool TcpSocket_isConnected(const TcpSocket *sock) {
    assert(sock->handle >= 0);
    return sock->isConnected;
}

bool TcpSocket_tryConnectIpv4(
        TcpSocket *sock, u8 ip_a, u8 ip_b, u8 ip_c, u8 ip_d, u32 port) {
    assert(sock->handle >= 0 && "Socket must be initialized");
    assert(!sock->isConnected && "Socket is already connected");

    sock->addr.len = sizeof(sock->addr);
    sock->addr.family = SO_PF_INET;
    sock->addr.port = port;
    sock->addr.addr.addr = (ip_a << 24) | (ip_b << 16) | (ip_c << 8) | (ip_d << 0);

    TCP_VERBOSE_LOG("[TCP] SOConnect..\n");
    const int res = SOConnect(sock->handle, &sock->addr);
    TCP_VERBOSE_LOG("[TCP] ..SOConnect returned %i\n", res);

    TCP_EARLY_ASSERT(res == 0 && "Failed to connect");
    sock->isConnected = res == 0;

    return sock->isConnected;
}

void TcpSocket_disconnect(TcpSocket *sock) {
    assert(sock->handle >= 0 && "Socket isn't initialized.");
    assert(sock->isConnected && "Socket isn't connected.");

    {
        const int res = SOShutdown(sock->handle, SO_SHUT_RDWR);
        assert(res == 0 && "Socket failed to shutdown");
    }

    {
        const int res = SOClose(sock->handle);
        assert(res == 0 && "Socket failed to close");
    }

    sock->isConnected = false;
}

bool TcpSocket_receiveBytes(TcpSocket *sock, void *buf, u32 len) {
    TCP_VERBOSE_LOG("SORecv %p, %u\n", buf, len);
    const u32 res = SORecv(sock->handle, buf, len, 0 /* , &addr */);
    TCP_VERBOSE_LOG("SORecv returned %i\n", res);

    TCP_EARLY_ASSERT(res == len && "Socket RECV failed");
    return res == len;
}
bool TcpSocket_sendBytes(TcpSocket *sock, const void *buf, u32 len) {
    TCP_VERBOSE_LOG("SOSend %p, %u\n", buf, len);
    const s32 res = SOSend(sock->handle, buf, len, 0 /* , &addr */);
    TCP_VERBOSE_LOG("SOSend returned %i\n", res);

    TCP_EARLY_ASSERT((u32)res == len && "Socket SEND failed");
    return (u32)res == len;
}
