#include "LicenseSelectButton.h"

#include "LicenseSelectPage.h"
#include "SectionManager.h"

#include "../system/SaveManager.h"

#include <stdio.h>
#include <string.h>

static void onFront(PushButtonHandler *this, PushButton *base, u32 localPlayerId) {
    UNUSED(this);
    UNUSED(localPlayerId);

    LicenseSelectButton *button = (LicenseSelectButton *)base;

    u32 index = button->index;
    if (index < s_saveManager->spLicenseCount) {
        s_saveManager->spCurrentLicense = index;
        const Mii *mii = MiiGroup_get(&button->miiGroup, 0);
        if (mii) {
            SaveManager_createLicense(s_saveManager, 0, &mii->id, mii->name);
            SaveManager_selectLicense(s_saveManager, 0);
            GlobalContext *cx = s_sectionManager->globalContext;
            MiiGroup_copy(&cx->localPlayerMiis, &button->miiGroup, 0, 0);
            LicenseSelectPage *page = (LicenseSelectPage *)button->group->page;
            page->replacement = 0x5a; // TODO enum
            f32 delay = PushButton_getDelay(button);
            Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
        } else {
            Section *currentSection = s_sectionManager->currentSection;
            ConfirmPage *confirmPage = (ConfirmPage *)currentSection->pages[PAGE_ID_CONFIRM];
            ConfirmPage_reset(confirmPage);
            ConfirmPage_setTitleMessage(confirmPage, 0x898, NULL);
            ConfirmPage_setWindowMessage(confirmPage, 0x89d, NULL);
            confirmPage->onConfirm = &button->onChangeConfirm;
            confirmPage->onCancel = &button->onCancel;
            LicenseSelectPage *page = (LicenseSelectPage *)button->group->page;
            page->replacement = PAGE_ID_CONFIRM;
            f32 delay = PushButton_getDelay(button);
            Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
        }
    } else if (index == s_saveManager->spLicenseCount) {
        Section *currentSection = s_sectionManager->currentSection;
        ConfirmPage *confirmPage = (ConfirmPage *)currentSection->pages[PAGE_ID_CONFIRM];
        ConfirmPage_reset(confirmPage);
        ConfirmPage_setTitleMessage(confirmPage, 0x836, NULL);
        ConfirmPage_setWindowMessage(confirmPage, 0x835, NULL);
        confirmPage->onConfirm = &button->onCreateConfirm;
        confirmPage->onCancel = &button->onCancel;
        LicenseSelectPage *page = (LicenseSelectPage *)button->group->page;
        page->replacement = PAGE_ID_CONFIRM;
        f32 delay = PushButton_getDelay(button);
        Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
    }
}

static const PushButtonHandler_vt onFront_vt = {
    .handle = onFront,
};

static void onCreateConfirm(ConfirmPageHandler *this, ConfirmPage *confirmPage, f32 delay) {
    UNUSED(this);
    UNUSED(confirmPage);

    SectionManager_setNextSection(s_sectionManager, 0x45 /* TODO enum */, PAGE_ANIMATION_NEXT);
    SectionManager_startChangeSection(s_sectionManager, delay, 0x000000ff);
}

static const ConfirmPageHandler_vt onCreateConfirm_vt = {
    .handle = onCreateConfirm,
};

static void onChangeConfirm(ConfirmPageHandler *this, ConfirmPage *confirmPage, f32 delay) {
    UNUSED(this);
    UNUSED(confirmPage);

    SectionManager_setNextSection(s_sectionManager, 0x46 /* TODO enum */, PAGE_ANIMATION_NEXT);
    SectionManager_startChangeSection(s_sectionManager, delay, 0x000000ff);
}

static const ConfirmPageHandler_vt onChangeConfirm_vt = {
    .handle = onChangeConfirm,
};

static void onCancel(ConfirmPageHandler *this, ConfirmPage *confirmPage, f32 delay) {
    UNUSED(this);
    UNUSED(delay);

    confirmPage->replacement = 0x65; // TODO enum
}

static const ConfirmPageHandler_vt onCancel_vt = {
    .handle = onCancel,
};

LicenseSelectButton *LicenseSelectButton_ct(LicenseSelectButton *this) {
    PushButton_ct(this);

    MiiGroup_ct(&this->miiGroup);
    this->onFront.vt = &onFront_vt;
    this->onCreateConfirm.vt = &onCreateConfirm_vt;
    this->onChangeConfirm.vt = &onChangeConfirm_vt;
    this->onCancel.vt = &onCancel_vt;

    return this;
}

void LicenseSelectButton_dt(LicenseSelectButton *this, s32 type) {
    MiiGroup_dt(&this->miiGroup, -1);

    PushButton_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

void LicenseSelectButton_load(LicenseSelectButton *this, u32 index) {
    char variant[0x9];
    snprintf(variant, sizeof(variant), "License%lu", index);
    PushButton_load(this, "button", "LicenseSelect", variant, 0x1, false, false);
    MiiGroup_init(&this->miiGroup, 1, 0x1, NULL);
    if (index < s_saveManager->spLicenseCount) {
        LayoutUIControl_setPaneVisible(this, "new", false);
        MiiGroup_insertFromId(&this->miiGroup, 0, &s_saveManager->spLicenses[index]->miiId);
        LayoutUIControl_setPaneVisible(this, "mii", true);
        LayoutUIControl_setMiiPicture(this, "mii", &this->miiGroup, 0, 0);
        ExtendedMessageInfo info = {
            .miis[0] = MiiGroup_get(&this->miiGroup, 0),
        };
        LayoutUIControl_setMessage(this, "player", 0x251d, &info);
    } else if (index == s_saveManager->spLicenseCount) {
        LayoutUIControl_setPaneVisible(this, "new", true);
        LayoutUIControl_setMessage(this, "new", 0x1781, NULL);
        LayoutUIControl_setPaneVisible(this, "mii", false);
    } else {
        this->isHidden = true;
        PushButton_setPlayerFlags(this, 0x0);
    }
    this->index = index;
    PushButton_setFrontHandler(this, &this->onFront, false);
}
