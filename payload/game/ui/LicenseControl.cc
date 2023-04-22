#include "LicenseControl.hh"

namespace UI {

void LicenseControl::Refresh(void * /* r3 */, LicenseControl *control, u32 /* licenseId */,
        MiiGroup *miiGroup, u32 index) {
    control->setPaneVisible("new", false);
    control->setPaneVisible("mii", true);
    control->setMiiPicture("mii", miiGroup, index, 0);

    MessageInfo info;
    info.miis[0] = miiGroup->get(index);
    control->setMessage("player", 9501, &info);
}

} // namespace UI
