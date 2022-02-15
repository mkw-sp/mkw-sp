#include "SectionManager.h"

PATCH_S16(SectionManager_init, 0x8e, sizeof(GlobalContext));

void my_SectionManager_startChangeSection(SectionManager *menu, s32 delay, u32 color) {
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
