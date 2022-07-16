#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/UIControl.hh"

namespace UI {

class SheetSelectControl : public UIControl {
public:
    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(SheetSelectControl *control, u32 localPlayerId) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(SheetSelectControl *, u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(SheetSelectControl *control, u32 localPlayerId) override {
            (m_object->*m_function)(control, localPlayerId);
        }

    private:
        T *m_object;
        void (T::*m_function)(SheetSelectControl *, u32);
    };

    SheetSelectControl();
    ~SheetSelectControl() override;
    void dt(s32 type) override;

    void load(const char *dir, const char *rightFile, const char *rightVariant,
            const char *leftFile, const char *leftVariant, u32 playerFlags, bool r10,
            bool pointerOnly);
    void setRightHandler(IHandler *handler);
    void setLeftHandler(IHandler *handler);
    void setPlayerFlags(u32 playerFlags);
    void setPointerOnly(bool pointerOnly);

private:
    class SheetSelectButton : public LayoutUIControl {
    public:
        SheetSelectButton();
        ~SheetSelectButton() override;

        void setPointerOnly(bool pointerOnly);

    private:
        ControlInputManager m_inputManager;
        u8 _1f8[0x248 - 0x1f8];
    };
    static_assert(sizeof(SheetSelectButton) == 0x248);

    u8 _098[0x0a8 - 0x098];
    SheetSelectButton m_rightButton;
    SheetSelectButton m_leftButton;
};
static_assert(sizeof(SheetSelectControl) == 0x538);

} // namespace UI
