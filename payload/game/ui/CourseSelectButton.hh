#pragma once

#include "Button.hh"

#include <sp/TrackPackManager.hh>

namespace UI {

class CourseSelectButton : public PushButton {
public:
    CourseSelectButton();
    ~CourseSelectButton();

    void load(u32 i);
    void refresh(Sha1 dbId);
    void setTex(u8 c, const GXTexObj &texObj);
};

} // namespace UI
