#include "CourseMap.hh"

namespace System {

const MapdataKartPointAccessor *CourseMap::kartPoint() const {
    return m_kartPoint;
}

CourseMap *CourseMap::Instance() {
    return s_instance;
}

} // namespace System
