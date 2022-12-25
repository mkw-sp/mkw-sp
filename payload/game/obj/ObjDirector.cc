#include "ObjDirector.hh"

#include <sp/3d/DrawDebug.hh>

namespace Geo {

void ObjDirector::drawDebug() {
    SP::DrawDebug(false);
    REPLACED(drawDebug)();
}

ObjDirector *ObjDirector::Instance() {
    return s_instance;
}

} // namespace Geo
