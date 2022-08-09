#include <revolution.h>
#include <revolution/ios.h>

#include <sp/Bytes.h>

#include <string.h>

void *SOiAlloc(u32 id, s32 size);
void SOiFree(u32 id, void *buf, s32 size);
int SOiPrepare(u32 r3, s32 *fd);
int SOiConclude(u32 r3, s32 result);

int SOListen(int s, int backlog) {
    s32 fd;
    s32 result = SOiPrepare(0, &fd);
    if (result != 0) {
        return result;
    }
    void *buf = SOiAlloc(0xc, 0x20);
    if (!buf) {
        result = -0x31;
    } else {
        write_u32(buf, 0, s);
        write_u32(buf, 4, backlog);
        result = IOS_Ioctl(fd, 0xa, buf, 8, NULL, 0);
        SOiFree(0xc, buf, 0x20);
    }
    return SOiConclude(0, result);
}

int SOAccept(int s, void *sockAddr) {
    s32 fd;
    s32 result = SOiPrepare(0, &fd);
    if (result != 0) {
        return result;
    }
    SOSockAddrIn *addrIn = sockAddr;
    if (addrIn && addrIn->len != 8) {
        return -0x1c;
    }
    u8 len = OSRoundUp32B(addrIn ? addrIn->len + 0x20 : 4);
    void *buf = SOiAlloc(0xc, len);
    if (!buf) {
        result = -0x31;
    } else {
        write_u32(buf, 0, s);
        if (addrIn) {
            memcpy(buf + 0x20, addrIn, addrIn->len);
            result = IOS_Ioctl(fd, 0x1, buf, 4, buf + 0x20, addrIn->len);
            if (result >= 0) {
                memcpy(sockAddr, buf + 0x20, 8);
            }
        } else {
            result = IOS_Ioctl(fd, 0x1, buf, 4, NULL, 0);
        }
        SOiFree(0xc, buf, len);
    }
    return SOiConclude(0, result);
}
