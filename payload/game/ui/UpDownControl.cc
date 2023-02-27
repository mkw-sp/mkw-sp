#include "UpDownControl.hh"

namespace UI {

UpDownAnimator::UpDownAnimator() = default;

UpDownAnimator::~UpDownAnimator() = default;

UpDownControl::UpDownButton::UpDownButton() = default;

UpDownControl::UpDownButton::~UpDownButton() = default;

UpDownControl::UpDownControl() = default;

UpDownControl::~UpDownControl() = default;

u32 UpDownControl::chosen() const {
    return m_chosen;
}

TextUpDownValueControl::TextControl::TextControl() = default;

TextUpDownValueControl::TextControl::~TextControl() = default;

TextUpDownValueControl::TextUpDownValueControl() = default;

TextUpDownValueControl::~TextUpDownValueControl() = default;

UpDownAnimator *TextUpDownValueControl::animator() {
    return &m_animator;
}

TextUpDownValueControl::TextControl *TextUpDownValueControl::shownText() {
    return m_shownText;
}

TextUpDownValueControl::TextControl *TextUpDownValueControl::hiddenText() {
    return m_hiddenText;
}

} // namespace UI
