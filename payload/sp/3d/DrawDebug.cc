#include "DrawDebug.hh"
#include "Checkpoints.hh"
#include "Kcl.hh"
#include <game/race/FieldDirector.hh>
#include <game/render/DrawList.hh>
#include <game/system/ResourceManager.hh>
#include <game/system/SaveManager.hh>
#include <sp/settings/ClientSettings.hh>

namespace SP {

// TODO: This code being in the SP folder doesn't make too much sense I think.

static KclVis *s_kclVis;
static bool s_isHidden = false;

void DrawDebug(bool opa) {
    const std::array<float, 12> mtx = Render::DrawList::spInstance->getViewMatrix();
    auto *saveManager = System::SaveManager::Instance();
    if (opa) {
        using DebugKCL = SP::ClientSettings::DebugKCL;
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DebugKCL>();
        if (setting == DebugKCL::Replace && !s_isHidden) {
            SP_LOG("Hiding course");
            Race::FieldDirector::Instance()->hideCourse();
            s_isHidden = true;
        } else if (setting != DebugKCL::Replace && s_isHidden) {
            SP_LOG("Showing course");
            Race::FieldDirector::Instance()->showCourse();
            s_isHidden = false;
        }
        if (setting == DebugKCL::Overlay || setting == DebugKCL::Replace) {
            if (s_kclVis) {
                s_kclVis->render(Decay(mtx), setting == DebugKCL::Overlay);
            }
        }
    } else {
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DebugCheckpoints>();
        if (setting == SP::ClientSettings::DebugCheckpoints::Enable) {
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
    SP_LOG("Allocating KclVis (%u bytes)", sizeof(KclVis));
    std::span<const u8> bytes{(u8 *)file, (u8 *)file + 10000000 /* act of faith */};
    s_kclVis = new KclVis(bytes);
}

void REPLACED(KCLManager_dsi)(void *mgr);
REPLACE void KCLManager_dsi(void *mgr) {
    Race::FieldDirector::Instance()->showCourse();
    s_isHidden = false;
    REPLACED(KCLManager_dsi)(mgr);
    SP_LOG("Freeing KclVis (%u bytes)", sizeof(KclVis));
    delete s_kclVis;
    s_kclVis = nullptr;
}

} // namespace SP
