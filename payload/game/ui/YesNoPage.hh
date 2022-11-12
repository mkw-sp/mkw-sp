#pragma once

#include "game/ui/Button.hh"
#include "game/ui/Page.hh"

namespace UI {

class YesNoPage : public Page {
public:
    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(s32 choice, PushButton *button) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(s32, PushButton *)) {
            m_object = object;
            m_function = function;
        }

        void handle(s32 choice, PushButton *button) override {
            (m_object->*m_function)(choice, button);
        }

    private:
        T *m_object;
        void (T::*m_function)(s32 choice, PushButton *button);
    };

    virtual void reset();

    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr);
    void configureButton(u32 index, u32 messageId, MessageInfo *info, Anim anim, IHandler *handler);

private:
    u8 _044[0x8b4 - 0x044];

protected:
    bool m_popRequested;

private:
    u8 _8b5[0x8b8 - 0x8b5];
};

class YesNoPagePopup : public YesNoPage {
public:
    void pop(Anim anim);

private:
    u8 _8b8[0xba0 - 0x8b8];
};
static_assert(sizeof(YesNoPagePopup) == 0xba0);

} // namespace UI
