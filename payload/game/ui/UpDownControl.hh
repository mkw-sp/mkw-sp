#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class UpDownAnimator {
public:
    UpDownAnimator();
    virtual ~UpDownAnimator();
    virtual void vf_08();
    virtual void vf_0c();
    virtual void vf_10();
    virtual void vf_14();
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();
    virtual void vf_24();
    virtual void vf_28();
    virtual void vf_2c();
    virtual void vf_30();
    virtual void vf_34();
    virtual void vf_38();
    virtual void vf_3c();
    virtual void vf_40();
    virtual void vf_44();
    virtual void vf_48();
    virtual void vf_4c();
    virtual void vf_50();
    virtual void vf_54();

private:
    void *_4 = nullptr;
};
static_assert(sizeof(UpDownAnimator) == 0x8);

class UpDownControl : public LayoutUIControl {
public:
    class UpDownButton : public LayoutUIControl {
    public:
        UpDownButton();
        ~UpDownButton() override;
        void init() override;
        void calc() override;
        void vf_28() override;
        void vf_2c() override;

    private:
        u8 _174[0x188 - 0x174];
    };
    static_assert(sizeof(UpDownButton) == 0x188);

    class IChangeHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(UpDownControl *control, u32 localPlayerId, u32 index) = 0;
    };

    template <typename T>
    class ChangeHandler : public IChangeHandler {
    public:
        ChangeHandler(T *object, void (T::*function)(UpDownControl *, u32, u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(UpDownControl *control, u32 localPlayerId, u32 index) override {
            (m_object->*m_function)(control, localPlayerId, index);
        }

    private:
        T *m_object;
        void (T::*m_function)(UpDownControl *, u32, u32);
    };

    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(UpDownControl *control, u32 localPlayerId) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(UpDownControl *, u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(UpDownControl *control, u32 localPlayerId) override {
            (m_object->*m_function)(control, localPlayerId);
        }

    private:
        T *m_object;
        void (T::*m_function)(UpDownControl *, u32);
    };

    UpDownControl();
    ~UpDownControl() override;
    void init() override;
    void calc() override;
    void vf_28() override;
    void vf_2c() override;
    virtual void vf_3c();
    virtual void vf_40();

    u32 chosen() const;
    // TODO stack_1b
    void load(u32 count, u32 chosen, const char *dir, const char *file, const char *variant,
            const char *rightFile, const char *rightVariant, const char *leftFile,
            const char *leftVariant, UpDownAnimator *animator, u32 playerFlags, bool stack_1b,
            bool pointerOnly, bool enableSide, bool enableFront);
    void setChangeHandler(IChangeHandler *handler);
    void setFrontHandler(IHandler *handler);
    void setSelectHandler(IHandler *handler);
    void selectDefault(u32 localPlayerId);
    void select(u32 localPlayerId);
    void reconfigure(u32 count, u32 chosen);
    void setPlayerFlags(u32 playerFlags);

private:
    void onSelect(u32 localPlayerId, u32 r5);
    void onDeselect(u32 localPlayerId, u32 r5);
    void onButtonSelect(u32 localPlayerId, u32 r5);
    void onButtonDeselect(u32 localPlayerId, u32 r5);
    void onFront(u32 localPlayerId, u32 r5);
    void onRight(u32 localPlayerId, u32 r5);
    void onLeft(u32 localPlayerId, u32 r5);

    template <typename T>
    using H = typename T::Handler<UpDownControl>;

    H<ControlInputManager> m_onSelect{this, &UpDownControl::onSelect};
    u8 _184[0x188 - 0x184];
    H<ControlInputManager> m_onDeselect{this, &UpDownControl::onDeselect};
    u8 _198[0x19c - 0x198];
    H<ControlInputManager> m_onButtonSelect{this, &UpDownControl::onButtonSelect};
    u8 _1ac[0x1b0 - 0x1ac];
    H<ControlInputManager> m_onButtonDeselect{this, &UpDownControl::onButtonDeselect};
    u8 _1c0[0x1c4 - 0x1c0];
    H<ControlInputManager> m_onFront{this, &UpDownControl::onFront};
    u8 _1d4[0x1d8 - 0x1d4];
    H<ControlInputManager> m_onRight{this, &UpDownControl::onRight};
    u8 _1e8[0x1ec - 0x1e8];
    H<ControlInputManager> m_onLeft{this, &UpDownControl::onLeft};
    u8 _1fc[0x200 - 0x1fc];
    u32 m_playerFlags;
    s32 m_chosen = -1;
    u32 m_count = 0;

public:
    u32 m_id = 0;

private:
    u8 _210[0x211 - 0x210];

public:
    bool m_enabled = true;

private:
    u8 _212[0x214 - 0x212];
    UpDownAnimator *m_animator = nullptr;
    ControlInputManager m_inputManager;
    IChangeHandler *m_changeHandler = nullptr;
    IHandler *m_frontHandler = nullptr;
    IHandler *m_selectHandler = nullptr;
    IHandler *m_deselectHandler = nullptr;
    u8 _2ac[0x2b4 - 0x2ac];
    UpDownButton m_buttons[2];
    u32 _5c4 = 19;
};
static_assert(sizeof(UpDownControl) == 0x5c8);

class TextUpDownValueControl : public LayoutUIControl {
public:
    class TextControl : public LayoutUIControl {
    public:
        TextControl();
        ~TextControl() override;
        void calcSelf() override;
        void vf_28() override;
        void vf_2c() override;
    };
    static_assert(sizeof(TextControl) == 0x174);

    class IChangeHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(TextControl *text, u32 index) = 0;
    };

    template <typename T>
    class ChangeHandler : public IChangeHandler {
    public:
        ChangeHandler(T *object, void (T::*function)(TextControl *, u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(TextControl *text, u32 index) override {
            (m_object->*m_function)(text, index);
        }

    private:
        T *m_object;
        void (T::*m_function)(TextControl *, u32);
    };

    TextUpDownValueControl();
    ~TextUpDownValueControl() override;
    void init() override;
    void calc() override;
    void vf_28() override;
    void vf_2c() override;

    UpDownAnimator *animator();
    TextControl *shownText();
    TextControl *hiddenText();
    void load(const char *dir, const char *file, const char *variant, const char *textFile,
            const char *textVariant);
    void setChangeHandler(IChangeHandler *handler);
    f32 getDelay() const;

private:
    UpDownAnimator m_animator;
    TextControl m_texts[2];
    TextControl *m_shownText;
    TextControl *m_hiddenText;
    u8 _46c[0x474 - 0x46c];
};
static_assert(sizeof(TextUpDownValueControl) == 0x474);

} // namespace UI
