#include "BalloonUnit.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/TeamColors.hh"

namespace Battle {

const char *BalloonUnit::vf_38() {
    m_drawMdl->m_hasColorAnim = true;

    return REPLACED(vf_38)();
}

void BalloonUnit::vf_48() {
    REPLACED(vf_48)();

    for (u32 i = 0; i < 2; i++) {
        auto *scnMdl = *m_drawMdl->scnMdl(i);
        m_copiedMatAccesses[i] = new nw4r::g3d::ScnMdl::CopiedMatAccess(scnMdl, 0);
        m_resMatTevColors[i] =
                new nw4r::g3d::ResMatTevColor(m_copiedMatAccesses[i]->GetResMatTevColor(false));
    }
}

void BalloonUnit::onAdd(u32 r4, u8 playerId, u8 r6, u8 r7) {
    REPLACED(onAdd)(r4, playerId, r6, r7);

    GXColor color0{255, 255, 255, 255};
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.spMaxTeamSize >= 2) {
        color0 = UI::TeamColors::Get(raceScenario.players[playerId].spTeam);
    }
    GXColor color1 = color0;
    color1.r /= 2;
    color1.g /= 2;
    color1.b /= 2;
    for (u32 i = 0; i < 2; i++) {
        m_resMatTevColors[i]->GXSetTevColor(GX_TEVREG0, color0);
        m_resMatTevColors[i]->GXSetTevColor(GX_TEVREG1, color1);
    }
}

} // namespace Battle
