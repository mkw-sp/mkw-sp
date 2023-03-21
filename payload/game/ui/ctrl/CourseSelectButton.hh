#pragma once

#include "Button.hh"

namespace UI {

class CourseSelectButton : public PushButton {
public:
    CourseSelectButton();
    ~CourseSelectButton();
    void calcSelf() override;

    void load(u32 i);
    void refresh(u32 messageId);
    void refresh(u8 c, const GXTexObj &texObj);

private:
    nw4r::lyt::Pane *m_panes[4];
    Vec2<f32> m_sizes[4];
};

} // namespace UI
