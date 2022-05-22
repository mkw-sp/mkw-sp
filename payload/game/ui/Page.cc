#include "Page.hh"

namespace UI {

Page::~Page() = default;

void Page::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~Page();
    }
}

void Page::setInputManager(MenuInputManager *inputManager) {
    m_inputManager = inputManager;
}

} // namespace UI
