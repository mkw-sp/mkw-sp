#include "DrawDebug.hh"
#include "Checkpoints.hh"
#include <game/render/DrawList.hh>
#include <game/system/SaveManager.hh>

extern "C" {
#include <revolution.h>
#include <revolution/gx.h>
}

#ifdef DRAW_PERF
extern "C" {
#include <sp/FlameGraph.h>
}
#endif // DRAW_PERF

#include <sp/settings/ClientSettings.hh>

#include "Kcl.hh"
#include <game/system/ResourceManager.hh>

#include <nw4r/g3d/g3d_resmdl_ac.hh>

namespace SP {

// TODO: This code being in the SP folder doesn't make too much sense I think.

KclVis *kcl;

#define DRAW_PERF(...)

// TODO: Proper name for this
struct GndMdlMgr {
    static GndMdlMgr *spInstance;
    u32 _;
    u32 _1;
    nw4r::g3d::ResFile course;
    nw4r::g3d::ResFile sky;
};

static bool isHidden = false;
void ShowModel() {
    if (!isHidden) {
        return;
    }
    isHidden = false;
    SP_LOG("Showing model");
    nw4r::g3d::ResFile &file = GndMdlMgr::spInstance->course;
    for (u32 i = 0; i < file.GetResMdlNumEntries(); ++i) {
        auto mdl = file.GetResMdl(i);
        for (u32 i = 0; i < mdl.GetResNodeNumEntries(); ++i) {
            mdl.GetResNode(i).SetVisibility(true);
        }
    }
}
void HideModel() {
    if (isHidden) {
        return;
    }
    isHidden = true;
    SP_LOG("Hiding model");
    nw4r::g3d::ResFile &file = GndMdlMgr::spInstance->course;
    for (u32 i = 0; i < file.GetResMdlNumEntries(); ++i) {
        auto mdl = file.GetResMdl(i);
        SP_LOG("MODEL NAME = %s", mdl.GetName());
        for (u32 i = 0; i < mdl.GetResNodeNumEntries(); ++i) {
            mdl.GetResNode(i).SetVisibility(false);
        }
    }
}

void DrawDebug(bool opa) {
    DRAW_PERF(SIMPLE_PERF);
    const std::array<float, 12> mtx = Render::DrawList::spInstance->getViewMatrix();
    auto *saveManager = System::SaveManager::Instance();
    if (opa) {
        using DebugKCL = SP::ClientSettings::DebugKCL;
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DebugKCL>();
        if (setting == DebugKCL::Replace) {
            HideModel();
        } else {
            ShowModel();
        }
        if (setting == DebugKCL::Overlay || setting == DebugKCL::Replace) {
            DRAW_PERF(SIMPLE_PERF_NAMED("KCL"));
            if (kcl) {
                kcl->render(Decay(mtx), setting == DebugKCL::Overlay);
            }
        }
    } else {
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DebugCheckpoints>();
        if (setting == SP::ClientSettings::DebugCheckpoints::Enable) {
            DRAW_PERF(SIMPLE_PERF_NAMED("Checkpoints"));
            DrawCheckpoints(Decay(mtx));
        }
    }
}

struct OpaquePass {};

void REPLACED(OpaquePass_Do)(OpaquePass *pass);
REPLACE void OpaquePass_Do(OpaquePass *pass) {
    DrawDebug(true);
    return REPLACED(OpaquePass_Do)(pass);
}

void REPLACED(KCLManager_fromFile)(void *mgr, void *file);
REPLACE void KCLManager_fromFile(void *mgr, void *file) {
    REPLACED(KCLManager_fromFile)(mgr, file);
    std::span<const u8> bytes{(u8 *)file, (u8 *)file + 10000000 /* act of faith */};
    kcl = new KclVis(bytes);
}

void REPLACED(KCLManager_dsi)(void *mgr);
REPLACE void KCLManager_dsi(void *mgr) {
    ShowModel();
    REPLACED(KCLManager_dsi)(mgr);
    delete kcl;
    kcl = nullptr;
}

} // namespace SP
