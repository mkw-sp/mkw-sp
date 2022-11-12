#include "YesNoPage.hh"

namespace UI {

void YesNoPagePopup::pop(Anim anim) {
    setAnim(anim);
    m_popRequested = true;
}

} // namespace UI
