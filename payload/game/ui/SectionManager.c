#include "SectionManager.h"

PATCH_S16(SectionManager_init, 0x8e, sizeof(GlobalContext));

static u32 processFadeColor(u32 color) {
    // Invert colors
    // NOTE: I think all callsites use 0x0000'00ff? So we could hardcode it.

    u32 inverted_color = color & 0xff;                        // Alpha
    inverted_color |= (0xff - ((color >> 8) & 0xff)) << 8;    // Blue
    inverted_color |= (0xff - ((color >> 16) & 0xff)) << 16;  // Green
    inverted_color |= (0xff - ((color >> 24) & 0xff)) << 24;  // Red

    return inverted_color;
}

void my_SectionManager_startChangeSection(SectionManager *menu, s32 delay, u32 color) {
    color = processFadeColor(color);

    switch (menu->state) {
    case IDLE:
        menu->state = CHANGE_REQUESTED;
        menu->changeTimer = delay;
        menu->fadeColor = color;
        break;
    case REINIT_REQUESTED:
    case CHANGE_REQUESTED:
        menu->state = CHANGE_REQUESTED;
        if (delay < menu->changeTimer) {
            menu->changeTimer = delay;
            menu->fadeColor = color;
        }
        break;
    case REINIT_READY:
        menu->state = OVERRIDE_READY;
        break;
    default:
        break;
    }
}

PATCH_B(SectionManager_startChangeSection, my_SectionManager_startChangeSection);
