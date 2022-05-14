#pragma once

#include "game/ui/ControlGroup.hh"
#include "game/ui/Font.hh"
#include "game/ui/Layout.hh"
#include "game/ui/UIAnimator.hh"

namespace UI {

class UIControl {
public:
    UIControl();

private:
    virtual void dummy_00() {}
    virtual void dummy_04() {}

public:
    virtual void dt(s32 type);
    virtual void init();
    virtual void calc();
    virtual void draw();

protected:
    virtual void initSelf();
    virtual void calcSelf();
    virtual void vf_20();
    virtual void vf_24();
    virtual void vf_28();
    virtual void vf_2c();
    virtual void vf_30();
    virtual void vf_34();

private:
    u8 _04[0x64 - 0x04];
    ControlGroup *m_controlGroup;
    u8 _68[0x80 - 0x68];
    bool m_isHidden;
    u8 _81[0x98 - 0x81];
};
static_assert(sizeof(UIControl) == 0x98);

class LayoutUIControl : public UIControl {
public:
    LayoutUIControl();
    void dt(s32 type) override;
    void init() override;
    void calc() override;
    void draw() override;

protected:
    void vf_28() override;
    void vf_2c() override;
    void vf_30() override;
    virtual void vf_38();

private:
    UIAnimator m_animator;
    MainLayout m_mainLayout;
    u8 _144[0x174 - 0x144];
};
static_assert(sizeof(LayoutUIControl) == 0x174);

} // namespace UI
