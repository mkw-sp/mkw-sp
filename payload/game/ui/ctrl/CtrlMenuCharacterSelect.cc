#include "game/ui/ctrl/CtrlMenuCharacterSelect.hh"

namespace UI {

void CtrlMenuCharacterSelect::ButtonDriver::initSelf() {
    REPLACED(initSelf)();

    if (auto *pane = m_mainLayout.findPaneByName("ok_text")) {
        if (auto *material = pane->getMaterial()) {
            material->tevColors[1] = { 255, 255, 255, 255 };
        }
    }

    if (auto *pane = m_mainLayout.findPaneByName("ok_text_1p")) {
        if (auto *material = pane->getMaterial()) {
            material->tevColors[1] = { 255, 255, 255, 255 };
        }
        setMessage("ok_text_1p", 2555);
    }

    if (auto *pane = m_mainLayout.findPaneByName("ok_text_2p")) {
        if (auto *material = pane->getMaterial()) {
            material->tevColors[1] = { 255, 255, 255, 255 };
        }
        setMessage("ok_text_2p", 2555 + 1);
    }

    if (auto *pane = m_mainLayout.findPaneByName("ok_text_3p")) {
        if (auto *material = pane->getMaterial()) {
            material->tevColors[1] = { 255, 255, 255, 255 };
        }
        setMessage("ok_text_3p", 2555 + 2);
    }

    if (auto *pane = m_mainLayout.findPaneByName("ok_text_4p")) {
        if (auto *material = pane->getMaterial()) {
            material->tevColors[1] = { 255, 255, 255, 255 };
        }
        setMessage("ok_text_4p", 2555 + 3);
    }
}

} // namespace UI
