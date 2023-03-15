#include "TopMenuPage.h"

#include "game/system/SaveManager.h"

#define MAX_SP_LICENSE_COUNT 6

// Hide the channel button
PATCH_S16(TopMenuPage_ct, 0x7e, 3);

void TopMenuPage_initMiiGroup(TopMenuPage *this) {
    this->inherit.miiGroup = new(sizeof(MiiGroup));
    MiiGroup_ct(this->inherit.miiGroup);
    MiiGroup_init(this->inherit.miiGroup, MAX_SP_LICENSE_COUNT, 0x4, NULL);
    for (u32 i = 0; i < SaveManager_SPLicenseCount(); i++) {
        MiiId miiId = SaveManager_GetSPLicenseMiiId(i);
        MiiGroup_insertFromId(this->inherit.miiGroup, i, &miiId);
    }
}

void TopMenuPage_refreshFileAdminButton(TopMenuPage *this) {
    u32 index = SaveManager_SPCurrentLicense();
    LayoutUIControl_setMiiPicture(&this->fileAdminButton->inherit, "mii", this->inherit.miiGroup, index, 2);
}

void TopMenuPage_onButtonSelect(TopMenuPage *this, PushButton *button);

void my_TopMenuPage_onButtonSelect(TopMenuPage *this, PushButton *button) {
    if (button->index == -100) {
        CtrlMenuInstructionText_setMessage(this->inherit.instructionText, 0, NULL);
    } else {
        const u32 messageIds[] = {
            10073,
            2021,
            2022,
            2023,
            20001,
            20002,
            2026,
            10074,
        };
        CtrlMenuInstructionText_setMessage(this->inherit.instructionText, messageIds[button->index], NULL);
    }
}
PATCH_B(TopMenuPage_onButtonSelect, my_TopMenuPage_onButtonSelect);
