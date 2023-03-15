#include "ios.h"

#include <revolution.h>

static bool CanIOSOpen(const char *path, u32 flags) {
    (void)path;
    (void)flags;

    return true;
}

s32 my_IOS_Open(const char *path, u32 flags) {
    if (!CanIOSOpen(path, flags)) {
        SP_LOG("-> Blocked opening %s (%x)", path, flags);
        return -1;
    }

    const s32 result = real_IOS_Open(path, flags);
    if ((SP_DEBUG_LEVEL & SP_DEBUG_IOS_OPENS) == SP_DEBUG_IOS_OPENS) {
        SP_LOG("IOS_Open: %s (%x) -> result 0x%x", path, flags, result);
    }
    return result;
}

s32 my_IOS_OpenAsync(const char *path, u32 flags, void *cb, void *userdata) {
    if (!CanIOSOpen(path, flags)) {
        SP_LOG("-> Blocked opening %s (%x)", path, flags);
        return -1;
    }

    const s32 result = real_IOS_OpenAsync(path, flags, cb, userdata);
    if ((SP_DEBUG_LEVEL & SP_DEBUG_IOS_OPENS) == SP_DEBUG_IOS_OPENS) {
        SP_LOG("IOS_OpenAsync: %s (%x) cb=%p, user=%p -> result 0x%x", path, flags, cb, userdata,
                result);
    }
    return result;
}
