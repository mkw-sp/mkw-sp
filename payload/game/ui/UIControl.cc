#include "UIControl.hh"

namespace UI {

UIControl::~UIControl() {}

void UIControl::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~UIControl();
    }
}

LayoutUIControl::~LayoutUIControl() {
    m_commonMessageGroup.dt(-1);
    m_specificMessageGroup.dt(-1);
    m_mainLayout.dt(-1);
    m_animator.dt(-1);
}

} // namespace UI
