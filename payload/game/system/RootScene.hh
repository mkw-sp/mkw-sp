#include "game/host_system/Scene.hh"

namespace System {

class RootScene : public Scene {
public:
    void allocate();

    static RootScene *Instance() {
        return s_instance;
    }

private:
    u8 _c70[0xcb8 - 0xc70];

    static RootScene *s_instance;
};

static_assert(sizeof(RootScene) == 0xcb8);

} // namespace System
