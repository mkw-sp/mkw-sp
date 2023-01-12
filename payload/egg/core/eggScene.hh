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

    Scene *getParent() const {
        return m_parent;
    }

    Scene *getChild() const {
        return m_child;
    }

    u32 getSceneID() const {
        return m_sceneID;
    }

private:
    u8 _04[0x20 - 0x04];
    Scene *m_parent;
    Scene *m_child;
    u32 m_sceneID;
    u8 _2c[0x30 - 0x2c];
};
static_assert(sizeof(Scene) == 0x30);

} // namespace EGG
