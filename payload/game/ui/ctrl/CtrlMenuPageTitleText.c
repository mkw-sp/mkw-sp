#include "CtrlMenuPageTitleText.h"

extern const LayoutUIControl_vt s_CtrlMenuPageTitleText_vt;

CtrlMenuPageTitleText *CtrlMenuPageTitleText_ct(CtrlMenuPageTitleText *this) {
    LayoutUIControl_ct(this);
    this->vt = &s_CtrlMenuPageTitleText_vt;
    return this;
}
