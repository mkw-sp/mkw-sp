#include "CtrlMenuInstructionText.h"

extern const LayoutUIControl_vt s_CtrlMenuInstructionText_vt;

CtrlMenuInstructionText *CtrlMenuInstructionText_ct(CtrlMenuInstructionText *this) {
    LayoutUIControl_ct(this);
    this->vt = &s_CtrlMenuInstructionText_vt;
    return this;
}
