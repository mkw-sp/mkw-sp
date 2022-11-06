#include "ObjDirector.hh"

#include <sp/3d/DrawDebug.hh>

namespace Geo {

void ObjDirector::drawDebug() {
    SP::DrawDebug();
    REPLACED(drawDebug)();
}

} // namespace Geo
