#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class PushButton : public LayoutUIControl {
public:
    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(PushButton *button, u32 localPlayerId) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(PushButton *, u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(PushButton *button, u32 localPlayerId) override {
            (m_object->*m_function)(button, localPlayerId);
        }

    private:
        T *m_object;
        void (T::*m_function)(PushButton *, u32);
    };

    PushButton();
    ~PushButton() override;
    void init() override;
    void calc() override;
    void vf_28() override;
    void vf_2c() override;
    virtual void vf_3c();
    virtual void vf_40();
    virtual void vf_44();
    virtual void vf_48();

    void load(const char *dir, const char *file, const char *variant, u32 playerFlags, bool r8,
            bool pointerOnly);
    void setFrontHandler(IHandler *handler, bool repeat);
    void setSelectHandler(IHandler *handler, bool repeat);
    void setDeselectHandler(IHandler *handler, bool repeat);
    void selectDefault(u32 localPlayerId);
    void select(u32 localPlayerId);
    void setPlayerFlags(u32 playerFlags);
    void setPointerOnly(bool pointerOnly);
    f32 getDelay() const;
    bool isSelected() const;
    void setFrontSoundId(Sound::SoundId soundId);

    void onSelect(u32 localPlayerId, u32 r5);
    void onDeselect(u32 localPlayerId, u32 r5);
    void onFront(u32 localPlayerId, u32 r5);

private:
    template <typename T>
    using H = typename T::template Handler<PushButton>;

    ControlInputManager m_inputManager;
    H<ControlInputManager> m_onSelect{this, &PushButton::onSelect};
    u8 _204[0x208 - 0x204];
    H<ControlInputManager> m_onDeselect{this, &PushButton::onDeselect};
    u8 _218[0x21c - 0x218];
    H<ControlInputManager> m_onFront{this, &PushButton::onFront};
    u8 _230[0x234 - 0x230];
    IHandler *m_frontHandler = nullptr;
    IHandler *m_selectHandler = nullptr;
    IHandler *m_deselectHandler = nullptr;

public:
    s32 m_index = 0;

private:
    u32 m_playerFlags = 0x1;
    u8 _244[0x250 - 0x248];
    u32 _250 = 8;
};
static_assert(sizeof(PushButton) == 0x254);

} // namespace UI
