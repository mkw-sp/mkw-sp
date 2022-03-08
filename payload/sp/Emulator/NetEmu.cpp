//
// Windows implementation of TcpSocket + Net_init. When C++23 adds std networking, we can
// have one cross-platform backend.
//

extern "C" {
#include <sp/Net.h>
#include <sp/Tcp.h>
}

#include <stdio.h>
#include <cstring>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#else
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#define INVALID_SOCKET ~0
#endif

void TcpSocket_create(TcpSocket *sock) {
    sock->handle = INVALID_SOCKET;
    sock->isConnected = false;
}
void TcpSocket_destroy(TcpSocket *sock) {
    TcpSocket_disconnect(sock);
}

void TcpSocket_init(TcpSocket *sock) {}

bool TcpSocket_isConnected(const TcpSocket *sock) {
    return sock->isConnected;
}
bool TcpSocket_tryConnectIpv4(
        TcpSocket *sock, u8 ip_a, u8 ip_b, u8 ip_c, u8 ip_d, u32 port) {
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string ip = std::to_string(ip_a) + "." + std::to_string(ip_b) + "." +
                     std::to_string(ip_c) + "." + std::to_string(ip_d);
    std::string sport = std::to_string(port);

    struct addrinfo *result = NULL;
    {
        const int res = getaddrinfo(ip.c_str(), sport.c_str(), &hints, &result);
        if (res != 0) {
            SP_LOG("getaddrinfo failed: %d", res);
            return false;
        }
    }

    sock->handle = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    {
        const int res = connect(sock->handle, result->ai_addr, (int)result->ai_addrlen);
        if (res != 0) {
            TcpSocket_disconnect(sock);
            sock->handle = INVALID_SOCKET;
        }
    }

    // NOTE: The API will return multiple entries as a linked-list, but we don't check
    // them.

    freeaddrinfo(result);

    if (sock->handle == INVALID_SOCKET) {
        SP_LOG("Unable to connect to socket\n");
        return false;
    }

    return true;
}
void TcpSocket_disconnect(TcpSocket *sock) {
    if (sock->handle != INVALID_SOCKET) {
#ifdef _WIN32
        closesocket(sock->handle);
#else
        shutdown(sock->handle, SHUT_RDWR);
        close(sock->handle);
#endif
        sock->handle = INVALID_SOCKET;
    }
}

bool TcpSocket_receiveBytes(TcpSocket *sock, void *buf, u32 len) {
    assert(sock->handle != INVALID_SOCKET);
    return len == recv(sock->handle, (char *)buf, len, 0);
}
bool TcpSocket_sendBytes(TcpSocket *sock, const void *buf, u32 len) {
    assert(sock->handle != INVALID_SOCKET);
    return len == send(sock->handle, (char *)buf, len, 0);
}

#ifdef _WIN32
static void Net_shutdown() {
    WSACleanup();
}

static WSADATA wsaData;

bool Net_init(void) {
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
        SP_LOG("WSAStartup failed: %d", res);
        return false;
    }

    atexit(Net_shutdown);
    return true;
}
#else
bool Net_init(void) {
    return true;
}
#endif
