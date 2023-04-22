#pragma once

#include <nw4r/g3d/g3d_resmdl_ac.hh>

namespace Race {

class FieldDirector {
public:
    static FieldDirector *Instance();
    nw4r::g3d::ResFile course();

    void hideCourse();
    void showCourse();

private:
    static FieldDirector *s_instance;
    u32 _00;
    u32 _04;
    nw4r::g3d::ResFile m_courseResFile;
    nw4r::g3d::ResFile m_vrcornResFile;
};

} // namespace Race
