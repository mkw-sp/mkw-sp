#pragma once

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
    void selectDefault(u32 localPlayerId);
    void select(u32 localPlayerId);
    void setPlayerFlags(u32 playerFlags);
    f32 getDelay() const;

private:
    u8 _174[0x240 - 0x174];

public:
    s32 m_index;

private:
    u8 _244[0x254 - 0x244];
};
static_assert(sizeof(PushButton) == 0x254);

} // namespace UI
