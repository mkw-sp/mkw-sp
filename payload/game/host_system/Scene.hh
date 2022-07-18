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

} // namespace System
