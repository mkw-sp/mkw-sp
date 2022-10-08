#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class UpDownAnimator {
private:
    u8 _0[0x8 - 0x0];
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
    u8 _174[0x204 - 0x174];
    u32 m_chosen;
    u8 _208[0x20c - 0x208];

public:
    u32 m_id;

private:
    u8 _210[0x211 - 0x210];

public:
    bool m_enabled;

private:
    u8 _212[0x218 - 0x212];
    ControlInputManager m_inputManager;
    u8 _29c[0x2b4 - 0x29c];
    UpDownButton m_buttons[2];
    u8 _5c4[0x5c8 - 0x5c4];
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
