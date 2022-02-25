#pragma once

#include <Common.h>
#include <revolution.h>

typedef struct {
    s32 handle;
    SOSockAddrIn addr;
    bool isConnected;
} TcpSocket;

void TcpSocket_create(TcpSocket *sock);
void TcpSocket_destroy(TcpSocket *sock);

void TcpSocket_init(TcpSocket *sock);

bool TcpSocket_isConnected(const TcpSocket *sock);
bool TcpSocket_tryConnectIpv4(
        TcpSocket *sock, u8 ip_a, u8 ip_b, u8 ip_c, u8 ip_d, u32 port);
void TcpSocket_disconnect(TcpSocket *sock);

bool TcpSocket_receiveBytes(TcpSocket *sock, void *buf, u32 len);
bool TcpSocket_sendBytes(TcpSocket *sock, const void *buf, u32 len);
