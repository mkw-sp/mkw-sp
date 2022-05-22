#pragma once

#include "game/ui/ControlGroup.hh"
#include "game/ui/Font.hh"
#include "game/ui/Layout.hh"
#include "game/ui/MessageGroup.hh"
#include "game/ui/MiiGroup.hh"
#include "game/ui/UIAnimator.hh"

namespace UI {

class UIControl {
public:
    UIControl();
    virtual ~UIControl();
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

public:
    void setVisible(bool visible);

private:
    u8 _04[0x64 - 0x04];
    ControlGroup *m_controlGroup;
    ControlGroup m_children;
    u8 _7c[0x80 - 0x7c];
    bool m_isHidden;
    u8 _81[0x98 - 0x81];
};
static_assert(sizeof(UIControl) == 0x98);

class LayoutUIControl : public UIControl {
public:
    LayoutUIControl();
    ~LayoutUIControl() override;
    void init() override;
    void calc() override;
    void draw() override;

protected:
    void vf_28() override;
    void vf_2c() override;
    void vf_30() override;
    virtual void vf_38();

public:
    void setMessage(const char *pane, u32 messageId, MessageInfo *info);
    void setMessageAll(u32 messageId, MessageInfo *info);
    void setPicture(const char *dstPane, const char *srcPane);
    void setMiiPicture(const char *pane, MiiGroup *miiGroup, size_t index, u32 preset);
    void setPaneVisible(const char *pane, bool visible);

private:
    UIAnimator m_animator;
    MainLayout m_mainLayout;
    MessageGroup m_specificMessageGroup;
    MessageGroup m_commonMessageGroup;
    u8 _16c[0x174 - 0x16c];
};
static_assert(sizeof(LayoutUIControl) == 0x174);

} // namespace UI
