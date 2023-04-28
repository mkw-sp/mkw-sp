#pragma once

#include <Common.h>

#define SO_SHUT_RDWR 2

typedef void *(*SOAlloc)(u32 id, s32 size);
typedef void (*SOFree)(u32 id, void *buf, s32 size);
typedef struct SOLibraryConfig SOLibraryConfig;
struct SOLibraryConfig {
    SOAlloc alloc;
    SOFree free;
};
int SOStartup(void);
int SOCleanup(void);

int SOInit(const SOLibraryConfig *cfg);
int SOSend(s32 s, const void *buf, s32 len, s32 flags);
int SOSendTo(s32 s, const void *buf, s32 len, s32 flags, const void *sockFrom);
int SOWrite(s32 s, const void *buf, s32 len);

#define SO_PF_INET 2
#define SO_SOCK_STREAM 1
#define SO_SOCK_DGRAM 2

#define SO_SOL_SOCKET 0xffff

#define SO_SO_RCVBUF 0x00001002

#define SO_F_GETFL 3
#define SO_F_SETFL 4
#define SO_O_NONBLOCK 0x04

#define SO_EADDRINUSE -3
#define SO_EAGAIN -6
#define SO_EALREADY -7
#define SO_EINPROGRESS -26
#define SO_EISCONN -30

typedef struct SOInAddr {
    u32 addr;
} SOInAddr;

typedef struct SOSockAddrIn {
    u8 len;
    u8 family;
    u16 port;
    SOInAddr addr;
} SOSockAddrIn;

typedef struct SOAddrInfo {
    s32 flags;
    s32 family;
    s32 sockType;
    s32 protocol;
    unsigned addrLen;
    char *canonName;
    void *addr;
    struct SOAddrInfo *next;
} SOAddrInfo;

int SOSocket(int, int, int);
int SOClose(s32 s);

int SOListen(s32 s, s32 backlog);
int SOAccept(s32 s, void *sockAddr);
int SOBind(s32 s, const void *sockAddr);
int SOConnect(s32 s, const void *sockAddr);
int SOGetSockName(s32 s, void *sockAddr);
int SOGetPeerName(s32 s, void *sockAddr);
int SORecvFrom(s32 s, void *buf, s32 len, s32 flags, void *sockFrom);
int SORecv(s32 s, void *buf, s32 len, s32 flags);
int SORead(s32 s, void *buf, s32 len);

int SOSetSockOpt(s32 s, s32 level, s32 optname, const void *optval, s32 optlen);
int SOFcntl(s32 s, s32 cmd, ...);

int SOShutdown(s32 s, s32 how);

void SOFreeAddrInfo(SOAddrInfo *ai);
int SOGetAddrInfo(const char *nodeName, const char *servName, const SOAddrInfo *hints,
        SOAddrInfo **res);
