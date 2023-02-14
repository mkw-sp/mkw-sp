#pragma once

#include "game/render/DrawMdl.hh"

namespace Geo {

class Obj {
public:
    virtual ~Obj();
    virtual void dt(s32 type);
    virtual void vf_0c();
    virtual void vf_10();
    virtual void vf_14();
    virtual void vf_18();
    virtual void vf_1c();
    virtual void init() = 0;
    // ...

private:
    u8 _04[0x08 - 0x04];

protected:
    Render::DrawMdl *m_drawMdl;

private:
    u8 _0c[0xb0 - 0x0c];
};
static_assert(sizeof(Obj) == 0xb0);

// TODO better name?
class ObjEntity : public Obj {
public:
    ~ObjEntity() override;
    void init() override;
};

} // namespace Geo
