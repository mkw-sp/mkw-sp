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

private:
    u8 _098[0x538 - 0x098];
};
static_assert(sizeof(SheetSelectControl) == 0x538);

} // namespace UI
