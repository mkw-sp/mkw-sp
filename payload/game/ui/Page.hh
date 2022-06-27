#pragma once

#include "game/ui/ControlGroup.hh"
#include "game/ui/MenuInputManager.hh"
#include "game/ui/PageId.hh"
#include "game/ui/SectionId.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class Page {
public:
    enum class State {
        State0,
        State1,
        State2,
        State3,
        State4,
        State5,
    };

    enum class Anim {
        Next,
        Prev,
    };

    Page();
    virtual ~Page();
    virtual void dt(s32 type);
    virtual void vf_0c();
    virtual PageId getReplacement();
    virtual void vf_14();
    virtual void vf_18();
    virtual void changeSection(SectionId id, Anim anim, f32 delay);
    virtual void vf_20();
    virtual void push(PageId id, Anim anim);
    virtual void onInit();
    virtual void onDeinit();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void beforeInAnim();
    virtual void afterInAnim();
    virtual void beforeOutAnim();
    virtual void afterOutAnim();
    virtual void beforeCalc();
    virtual void afterCalc();
    virtual void vf_50();
    virtual void onRefocus();
    virtual void vf_58();
    virtual void vf_5c();
    virtual void vf_60();

protected:
    void setInputManager(MenuInputManager *inputManager);
    void skipInAnim();
    void skipOutAnim();

public:
    void setAnim(Anim anim);
    REPLACE void calc();

protected:
    void initChildren(u32 count);
    void insertChild(u32 index, UIControl *child, u32 drawPass);
    void startReplace(Anim anim, f32 delay);
    void calcAnim();
    void playSfx(u32 sfxId, s32 r5);
    void setAnimSfxIds(u32 nextId, u32 prevId);

private:
    PageId m_id;
    State m_state;
    bool m_canProceed;
    u8 _0d[0x24 - 0x0d];
    ControlGroup m_controlGroup;
    MenuInputManager *m_inputManager;
    u8 _3c[0x44 - 0x3c];
};
static_assert(sizeof(Page) == 0x44);

} // namespace UI
