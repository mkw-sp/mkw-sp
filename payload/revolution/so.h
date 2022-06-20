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
int SOSend(int s, const void* buf, int len, int flags);
int SOWrite(int s, const void* buf, int len);

#define SO_PF_INET 2
#define SO_SOCK_STREAM 1
#define SO_SOCK_DGRAM 2

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
    int flags;
    int family;
    int sockType;
    int protocol;
    unsigned addrLen;
    char *canonName;
    void *addr;
    struct SOAddrInfo *next;
} SOAddrInfo;

int SOSocket(int, int, int);
int SOClose(int s);

int SOListen(int s, int backlog);
int SOAccept(int s, void *sockAddr);
int SOBind(int s, const void *sockAddr);
int SOConnect(int s, const void *sockAddr);
int SOGetSockName(int s, void *sockAddr);
int SOGetPeerName(int s, void *sockAddr);
int SORecvFrom(int s, void *buf, int len, int flags, void *sockFrom);
int SORecv(int s, void *buf, int len, int flags);
int SORead(int s, void *buf, int len);

int SOShutdown(int s, int how);

void SOFreeAddrInfo(SOAddrInfo *ai);
int SOGetAddrInfo(const char *nodeName, const char *servName, const SOAddrInfo *hints,
        SOAddrInfo **res);
