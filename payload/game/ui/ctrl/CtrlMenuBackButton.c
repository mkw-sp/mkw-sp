#include "CtrlMenuBackButton.h"

enum {
    FADEOUT_STATE_DISABLED = 0,
    FADEOUT_STATE_ENABLED = 1,
};

extern const PushButton_vt s_CtrlMenuBackButton_vt;

CtrlMenuBackButton *CtrlMenuBackButton_ct(CtrlMenuBackButton *this) {
    PushButton_ct(this);
    this->vt = &s_CtrlMenuBackButton_vt;

    this->messageId = 2001;
    this->fadeoutState = FADEOUT_STATE_DISABLED;
    this->fadeoutOpacity = 0.0f;

    return this;
}
