#pragma once

#include <egg/core/eggScene.hh>

namespace System {

class Scene : public EGG::Scene {
public:
    Scene();
    ~Scene() override;

private:
    u8 _030[0xc70 - 0x030];
};
static_assert(sizeof(Scene) == 0xc70);

enum class RKSceneID {
    // --- <SceneCreatorREL>
    Title = 0,  //!< Called when REL first loaded
    Menu = 1,   //!< "メニューシーン" -> "Menu Scene"
    Race = 2,   //!< "レースシーン" -> "Race Scene"
    Globe = 4,  //!< "地球儀シーン" -> "Globe Scene"

    // --- <SceneCreatorStatic>
    Strap = 5,  //!< Root scene loaded immediately after initializing the system.
    // --- </SceneCreatorStatic>

    FlagOpen = 12,  //!< "フラグオープンシーン" -> "Flag Open Scene"
    // --- </SceneCreatorREL>
};

} // namespace System
