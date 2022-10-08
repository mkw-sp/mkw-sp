#include "game/ui/ctrl/CtrlMenuCharacterSelect.hh"

namespace UI {

void CtrlMenuCharacterSelect::ButtonDriver::initSelf() {
    REPLACED(initSelf)();

    if (auto *pane = m_mainLayout.findPaneByName("ok_text")) {
        if (auto *material = pane->getMaterial()) {
            material->tevColors[1] = { 255, 255, 255, 255 };
        }
    }
}

} // namespace UI
