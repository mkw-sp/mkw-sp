#pragma once

#include "game/ui/Page.hh"

namespace UI {

class MessagePage : public Page {
public:
    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(MessagePage *page) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(MessagePage *)) {
            m_object = object;
            m_function = function;
        }

        void handle(MessagePage *page) override {
            (m_object->*m_function)(page);
        }

    private:
        T *m_object;
        void (T::*m_function)(MessagePage *);
    };

    virtual void reset();
    virtual void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) = 0;
    virtual void vf_6c() = 0;

private:
    u8 _044[0x188 - 0x044];

public:
    IHandler *m_handler;

private:
    u8 _18c[0x1a5 - 0x18c];
};

class MessagePagePopup : public MessagePage {
public:
    void reset() override;
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) override;

private:
    u8 _1a5[0x604 - 0x1a5];
};
static_assert(sizeof(MessagePagePopup) == 0x604);

class MenuMessagePage : public MessagePage {
public:
    void reset() override;
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) override;

    void setTitleMessage(u32 messageId, MessageInfo *info = nullptr);

private:
    u8 _1a5[0x604 - 0x1a5];

public:
    PageId m_replacement;
};
static_assert(sizeof(MenuMessagePage) == 0x608);

} // namespace UI
