#pragma once

#include "game/sound/SoundId.hh"
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

    void initChildren(u32 count);
    void insertChild(u32 index, UIControl *child);
    void playSound(Sound::SoundId soundId, s32 r5);

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
    bool getVisible() const;
    const Page *getPage() const;
    Page *getPage();

private:
    u8 _04[0x64 - 0x04];
    ControlGroup *m_controlGroup;
    ControlGroup m_children;

public:
    f32 m_zIndex;

private:
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
    void setMessage(const char *pane, u32 messageId, MessageInfo *info = nullptr);
    void setMessageAll(u32 messageId, MessageInfo *info = nullptr);
    void setPicture(const char *dstPane, const char *srcPane);
    bool hasPictureSourcePane(const char *pane);
    void setMiiPicture(const char *pane, MiiGroup *miiGroup, size_t index, u32 preset);
    void setPaneVisible(const char *pane, bool visible);
    void load(const char *dir, const char *file,
        const char *variant, const char *const *groups);
    void setParentPane(const char *pane);

protected:
    UIAnimator m_animator;
    MainLayout m_mainLayout;

private:
    MessageGroup m_specificMessageGroup;
    MessageGroup m_commonMessageGroup;
    void *_16c = nullptr;
    void *_170 = nullptr;
};
static_assert(sizeof(LayoutUIControl) == 0x174);

class LayoutUIControlScaleFade : public LayoutUIControl {
public:
    LayoutUIControlScaleFade();
    ~LayoutUIControlScaleFade() override;
    void vf_20() override;
    void vf_28() override;
    void vf_2c() override;
};
static_assert(sizeof(LayoutUIControlScaleFade) == 0x174);

} // namespace UI
