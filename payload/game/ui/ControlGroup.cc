#include "ControlGroup.hh"

#include "game/ui/UIControl.hh"

namespace UI {

ControlGroup::~ControlGroup() {
    delete[] m_sortedControls;
    delete[] m_controls;
}

} // namespace UI
