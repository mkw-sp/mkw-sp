#include "FieldDirector.hh"

namespace Race {

FieldDirector *FieldDirector::Instance() {
    return s_instance;
}

nw4r::g3d::ResFile FieldDirector::course() {
    return m_courseResFile;
}

void FieldDirector::hideCourse() {
    for (u32 i = 0; i < m_courseResFile.GetResMdlNumEntries(); ++i) {
        auto mdl = m_courseResFile.GetResMdl(i);
        for (u32 i = 0; i < mdl.GetResNodeNumEntries(); ++i) {
            mdl.GetResNode(i).SetVisibility(false);
        }
    }
}
void FieldDirector::showCourse() {
    for (u32 i = 0; i < m_courseResFile.GetResMdlNumEntries(); ++i) {
        auto mdl = m_courseResFile.GetResMdl(i);
        for (u32 i = 0; i < mdl.GetResNodeNumEntries(); ++i) {
            mdl.GetResNode(i).SetVisibility(true);
        }
    }
}

} // namespace Race
