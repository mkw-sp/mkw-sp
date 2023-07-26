#include "RkSystem.hh"

#include <egg/core/eggXfbManager.hh>

#include <sp/MapFile.hh>
#include <sp/ScreenshotManager.hh>

namespace System {

void RkSystem::initialize() {
    REPLACED(initialize)();

    SP::MapFile::Load();

    auto *xfb = getXfbManager()->headXfb();
    assert(xfb);
    u32 framebufferSize = EGG::Xfb::CalcXfbSize(xfb->width(), xfb->height());
    SP::ScreenshotManager::CreateInstance(framebufferSize);
}

} // namespace System
