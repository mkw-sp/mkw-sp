#pragma once

#include <Common.hh>

namespace EGG {

class Scene {
public:
    Scene();
    virtual ~Scene();
    virtual void dt(s32 type);
    virtual void calc();
    virtual void draw();
    virtual void enter();
    virtual void exit();
    virtual void reinit();
    virtual void incoming_childDestroy();
    virtual void outgoing_childCreate();

private:
    u8 _04[0x30 - 0x04];
};
static_assert(sizeof(Scene) == 0x30);

} // namespace EGG
