#include "MainMenuPage.h"

#include "../../system/SaveManager.h"

void MainMenuPage_initMiiGroup(MainMenuPage *this) {
    this->miiGroup = new(sizeof(MiiGroup));
    MiiGroup_ct(this->miiGroup);
    MiiGroup_init(this->miiGroup, MAX_SP_LICENSE_COUNT, 0x4, NULL);
    for (u32 i = 0; i < s_saveManager->spLicenseCount; i++) {
        MiiGroup_insertFromId(this->miiGroup, i, &s_saveManager->spLicenses[i]->miiId);
    }
}

void MainMenuPage_refreshFileAdminButton(MainMenuPage *this) {
    u32 index = s_saveManager->spCurrentLicense;
    LayoutUIControl_setMiiPicture(this->fileAdminButton, "mii", this->miiGroup, index, 2);
}
