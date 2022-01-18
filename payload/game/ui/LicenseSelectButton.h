#pragma once

#include "Button.h"
#include "ConfirmPage.h"

typedef struct {
    PushButton;
    MiiGroup miiGroup;
    PushButtonHandler onFront;
    ConfirmPageHandler onCreateConfirm;
    ConfirmPageHandler onChangeConfirm;
    ConfirmPageHandler onCancel;
} LicenseSelectButton;

LicenseSelectButton *LicenseSelectButton_ct(LicenseSelectButton *this);

void LicenseSelectButton_dt(LicenseSelectButton *this, s32 type);

void LicenseSelectButton_load(LicenseSelectButton *this, u32 index);
