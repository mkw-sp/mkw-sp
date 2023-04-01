#pragma once

#include "Button.hh"

namespace UI {

class CourseSelectButton : public PushButton {
public:
    CourseSelectButton();
    ~CourseSelectButton();
    void calcSelf() override;

    void load(u32 i);
    void refresh(u32 courseId);
    void setTex(u8 c, const GXTexObj &texObj);

    u32 getWiimmId() const;

private:
    nw4r::lyt::Pane *m_panes[4];
    Vec2<f32> m_sizes[4];
    u32 m_wiimmId;
};

} // namespace UI
