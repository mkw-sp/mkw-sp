#pragma once

#include "game/sound/SoundId.hh"
#include "game/ui/ControlGroup.hh"
#include "game/ui/MenuInputManager.hh"
#include "game/ui/PageId.hh"
#include "game/ui/SectionId.hh"
#include "game/ui/TypeInfo.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class Page {
public:
    // TODO rename
    enum class State {
        State0,
        State1,
        State2,
        State3,
        State4,
        State5,
    };

    enum class Anim {
        None = -1,
        Next = 0,
        Prev = 1,
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
    virtual TypeInfo *getTypeInfo() const;

    template <typename P>
    const P *downcast() const {
        for (TypeInfo *typeInfo = getTypeInfo(); typeInfo != nullptr; typeInfo = typeInfo->base) {
            if (typeInfo == P::GetTypeInfo()) {
                return reinterpret_cast<const P *>(this);
            }
        }
        return nullptr;
    }

    template <typename P>
    P *downcast() {
        for (TypeInfo *typeInfo = getTypeInfo(); typeInfo != nullptr; typeInfo = typeInfo->base) {
            if (typeInfo == P::GetTypeInfo()) {
                return reinterpret_cast<P *>(this);
            }
        }
        return nullptr;
    }

protected:
    void setInputManager(MenuInputManager *inputManager);
    void skipInAnim();
    void skipOutAnim();

public:
    PageId id() const;
    State state() const;
    Anim anim() const;
    void setAnim(Anim anim);
    const MenuInputManager *inputManager() const;
    MenuInputManager *inputManager();
    REPLACE void calc();
    void deactivate();

    s32 frame() const {
        return m_frame;
    }

    void logControlsDebug();

protected:
    void initChildren(u32 count);
    void insertChild(u32 index, UIControl *child, u32 drawPass);
    void startReplace(Anim anim, f32 delay);
    void calcAnim();
    void playSound(Sound::SoundId soundId, s32 r5);
    void setAnimSfxIds(Sound::SoundId nextId, Sound::SoundId prevId);

private:
    PageId m_id;
    State m_state;
    bool m_canProceed;
    u8 _0d[0x10 - 0x0d];
    Anim m_anim;
    u8 _14[0x1c - 0x14];
    s32 m_frame;
    u8 _1c[0x24 - 0x20];
    ControlGroup m_controlGroup;
    MenuInputManager *m_inputManager;
    u8 _3c[0x44 - 0x3c];
};
static_assert(sizeof(Page) == 0x44);

} // namespace UI
