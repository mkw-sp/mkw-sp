#include "Es.h"

#include "Ios.h"

#include <stdalign.h>

enum {
    IOCTLV_LAUNCH_TITLE = 0x8,
};

static alignas(0x20) const char path[] = "/dev/es";
static alignas(0x20) u64 sTitleId;
static alignas(0x20) TicketView sView;
static alignas(0x20) IoctlvPair pairs[2];
static s32 fd = -1;

bool Es_init(void) {
    fd = Ios_open(path, 0);
    return fd >= 0;
}

void Es_launchTitle(u64 titleId, const TicketView *view) {
    sTitleId = titleId;
    sView = *view;

    pairs[0].data = &sTitleId;
    pairs[0].size = sizeof(sTitleId);
    pairs[1].data = &sView;
    pairs[1].size = sizeof(sView);

    Ios_ioctlvReboot(fd, IOCTLV_LAUNCH_TITLE, 2, pairs);
}

bool Es_deinit(void) {
    s32 result = Ios_close(fd);

    fd = -1;

    return result == 0;
}
