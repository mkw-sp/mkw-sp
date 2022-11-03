#include "game/ui/ctrl/CtrlMenuCharacterSelect.hh"

namespace UI {

void CtrlMenuCharacterSelect::ButtonDriver::initSelf() {
    REPLACED(initSelf)();

    if (auto *pane = m_mainLayout.findPaneByName("ok_text")) {
        if (auto *material = pane->getMaterial()) {
            material->tevColors[1] = {255, 255, 255, 255};
        }
    }

    const char *panes[4] = {"ok_text_1p", "ok_text_2p", "ok_text_3p", "ok_text_4p"};

    for (u32 i = 0; i < 4; i++) {
        if (auto *pane = m_mainLayout.findPaneByName(panes[i])) {
            if (auto *material = pane->getMaterial()) {
                material->tevColors[1] = {255, 255, 255, 255};
            }
            setMessage(panes[i], 2555 + i);
        }
    }
}

} // namespace UI
