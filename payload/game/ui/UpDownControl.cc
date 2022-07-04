#include "UpDownControl.hh"

namespace UI {

UpDownControl::UpDownButton::~UpDownButton() = default;

UpDownControl::~UpDownControl() = default;

u32 UpDownControl::chosen() const {
    return m_chosen;
}

TextUpDownValueControl::TextControl::~TextControl() = default;

TextUpDownValueControl::~TextUpDownValueControl() = default;

UpDownAnimator *TextUpDownValueControl::animator() {
    return &m_animator;
}

TextUpDownValueControl::TextControl *TextUpDownValueControl::shownText() {
    return m_shownText;
}

} // namespace UI
