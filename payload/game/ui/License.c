#include "UIControl.h"

typedef struct {
    LayoutUIControl;
} LicenseControl;

void refreshLicenseControl(void *r3, LicenseControl *control, u32 licenseId, MiiGroup *miiGroup, u32 index);

void my_refreshLicenseControl(void *r3, LicenseControl *control, u32 licenseId, MiiGroup *miiGroup, u32 index) {
    UNUSED(r3);
    UNUSED(licenseId);

    LayoutUIControl_setPaneVisible(control, "new", false);
    LayoutUIControl_setPaneVisible(control, "mii", true);
    LayoutUIControl_setMiiPicture(control, "mii", miiGroup, index, 0);
    ExtendedMessageInfo info = {
        .miis[0] = MiiGroup_get(miiGroup, index),
    };
    LayoutUIControl_setMessage(control, "player", 0x251d, &info);
}

PATCH_B(refreshLicenseControl, my_refreshLicenseControl);

