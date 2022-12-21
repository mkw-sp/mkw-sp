#pragma once

#include "game/system/GameScene.hh"

namespace Scene {

class RaceScene : public System::GameScene {
public:
    RaceScene();
    ~RaceScene() override;
    void vf_28() override;
    void vf_2c() override;
    REPLACE void calcSubsystems() override;
    void vf_34() override;
    void vf_38() override;
    void vf_3c() override;
    void vf_40() override;
    void vf_44() override;
    void vf_48() override;
    void vf_4c() override;
    void vf_50() override;
    void vf_54() override;
    void vf_58() override;

private:
    void calcSubsystems(s32 drift);

    u8 _254c[0x256c - 0x254c];
};
static_assert(sizeof(RaceScene) == 0x256c);

} // Scene
