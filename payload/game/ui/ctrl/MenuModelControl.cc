#include "MenuModelControl.hh"

#include "game/ui/PageId.hh"

namespace UI {

DriverModelControl::DriverModelControl() = default;

DriverModelControl::~DriverModelControl() = default;

void NoteModelControl::beforePageAnim(PageId pageId) {
    m_hasPageAnim = pageId == PageId::None || pageId == PageId::CourseSelect;
}

} // namespace UI
