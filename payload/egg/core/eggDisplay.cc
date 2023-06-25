#include "eggDisplay.hh"

#include "egg/core/eggSystem.hh"
#include "egg/core/eggVideo.hh"
#include "egg/core/eggXfbManager.hh"

#include <game/system/SaveManager.hh>
#include <sp/PerfOverlay.hh>

namespace EGG {

void Display::endFrame() {
    SP::PerfOverlay::MeasureEndCalc();

    GXDrawDone();
}

void Display::copyEFBtoXFB() {
    if (m_clear) {
        GXSetCopyClear(m_clearColor, m_clearZ);
    }

    auto *video = EGG::TSystem::Instance().getVideo();
    auto *renderMode = video->renderMode();
    GXBool aa = renderMode->aa ? GX_TRUE : GX_FALSE;
    GXBool vf = renderMode->aa ? GX_FALSE : GX_TRUE;
    if (vf == GX_TRUE && !renderMode->vi_xfb) {
        auto *saveManager = System::SaveManager::Instance();
        if (saveManager) {
            auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VerticalFilter>();
            if (setting == SP::ClientSettings::VerticalFilter::InterlacedOnly) {
                vf = GX_FALSE;
            }
        }
    }
    GXSetCopyFilter(aa, renderMode->sample, vf, renderMode->vert_filter);

    auto *xfbManager = EGG::TSystem::Instance().getXfbManager();
    xfbManager->copyEFB(m_clear);
}

} // namespace EGG
