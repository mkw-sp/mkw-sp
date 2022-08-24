#include "SheetSelectControl.hh"

namespace UI {

SheetSelectControl::~SheetSelectControl() = default;

void SheetSelectControl::setPointerOnly(bool pointerOnly) {
    m_rightButton.setPointerOnly(pointerOnly);
    m_leftButton.setPointerOnly(pointerOnly);
}

SheetSelectControl::SheetSelectButton::~SheetSelectButton() = default;

void SheetSelectControl::SheetSelectButton::setPointerOnly(bool pointerOnly) {
    m_inputManager.m_pointerOnly = pointerOnly;
}

SheetSelectControlScaleFade::SheetSelectControlScaleFade() = default;

SheetSelectControlScaleFade::~SheetSelectControlScaleFade() = default;

} // namespace UI
