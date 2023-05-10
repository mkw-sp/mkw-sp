#include "RkSystem.hh"

#include <sp/MapFile.hh>

namespace System {

void RkSystem::initialize() {
    REPLACED(initialize)();

    SP::MapFile::Load();
}

} // namespace System
