#include "es.h"

#include <revolution/ios.h>

#include <stdalign.h>

enum {
    IOCTLV_SIGN = 0x30,
};

extern s32 es_fd;

s32 ESP_Sign(const void *data, u32 size, u8 signature[0x3c], u8 certificate[0x180]) {
    if (!data != !size) {
        return -1017;
    }

    alignas(0x20) IOVector vec[3];
    vec[0].data = (void *)data;
    vec[0].size = size;
    vec[1].data = signature;
    vec[1].size = 0x3c;
    vec[2].data = certificate;
    vec[2].size = 0x180;

    return IOS_Ioctlv(es_fd, IOCTLV_SIGN, 1, 2, vec);
}
