#pragma once

#include "game/ui/Page.hh"

namespace UI {

class ConfirmPage : public Page {
public:
    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(ConfirmPage *page, f32 delay) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(ConfirmPage *, f32)) {
            m_object = object;
            m_function = function;
        }

        void handle(ConfirmPage *page, f32 delay) override {
            (m_object->*m_function)(page, delay);
        }

    private:
        T *m_object;
        void (T::*m_function)(ConfirmPage *, f32);
    };

    ConfirmPage();
    ~ConfirmPage() override;

    void reset();
    void setTitleMessage(u32 messageId, MessageInfo *info = nullptr);
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr);
    u32 choice() const;

private:
    u8 _044[0x820 - 0x044];

public:
    IHandler *m_confirmHandler;
    IHandler *m_cancelHandler;

private:
    u8 _828[0xa50 - 0x828];
    u32 m_choice;

public:
    PageId m_replacement;
};
static_assert(sizeof(ConfirmPage) == 0xa58);

} // namespace UI
