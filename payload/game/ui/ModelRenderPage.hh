#pragma once

#include "game/ui/Page.hh"
#include "game/util/Registry.hh"

namespace UI {

class ModelRenderPage : public Page {
public:
    void REPLACE onInit() override;

    void configure(u32 localPlayerId, bool r5, bool r6);
    void setCharacterId(u32 localPlayerId, Registry::Character characterId);
    void setVehicleId(u32 localPlayerId, Registry::Vehicle vehicleId);
    bool player_09(u8 localPlayerId) const;

    static u8 determineModelCount(SectionId sectionId);

    static void onDriverModelLoaded(s8 idx);

private:
    struct Player {
        Registry::Character m_characterId;
        Registry::Vehicle m_vehicleId;
        bool _08;
        bool _09;
    };

    MenuInputManager m_inputManager;
    Player m_players[4];
    u8 _82[0x84 - 0x82];
    u8 m_modelCount;
    u32 _88;
    u8 _8c[0x90 - 0x8c];
    u8 _90;
    u8 _91[0x1a14 - 0x91];
};
static_assert(sizeof(ModelRenderPage) == 0x1a14);

} // namespace UI
