#include "eggColorFader.h"

#include <revolution.h>

static bool post_fadeIn(bool changed) {
    if (changed) {
        axIsMuted = false;
    }

    return changed;
}

static bool post_fadeOut(bool changed) {
    if (changed) {
        axIsMuted = true;
    }

    return changed;
}

PATCH_B(EGG_ColorFader_fadeIn + 0x24, post_fadeIn);
PATCH_B(EGG_ColorFader_fadeOut + 0x28, post_fadeOut);
