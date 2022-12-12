#include "YesNoPage.hh"

namespace UI {

void YesNoPage::setDefaultChoice(u32 defaultChoice) {
    m_defaultChoice = defaultChoice;
}

void YesNoPagePopup::pop(Anim anim) {
    setAnim(anim);
    m_popRequested = true;
}

} // namespace UI
