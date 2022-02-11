#include "UIControl.h"

typedef struct {
    LayoutUIControl;
} LicenseControl;
static_assert(sizeof(LicenseControl) == 0x174);

void refreshLicenseControl(void *r3, LicenseControl *control, u32 licenseId, MiiGroup *miiGroup,
        u32 index);

void my_refreshLicenseControl(void *UNUSED(r3), LicenseControl *control, u32 UNUSED(licenseId),
        MiiGroup *miiGroup, u32 index) {
    LayoutUIControl_setPaneVisible(control, "new", false);
    LayoutUIControl_setPaneVisible(control, "mii", true);
    LayoutUIControl_setMiiPicture(control, "mii", miiGroup, index, 0);
    ExtendedMessageInfo info = {
        .miis[0] = MiiGroup_get(miiGroup, index),
    };
    LayoutUIControl_setMessage(control, "player", 9501, &info);
}
PATCH_B(refreshLicenseControl, my_refreshLicenseControl);
