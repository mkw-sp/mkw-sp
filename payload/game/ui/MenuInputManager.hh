#pragma once

extern "C" {
#include <Common.h>
}

namespace UI {

class MenuInputManager {
public:
    enum class InputId {
        Front,
        Back,
        Option,
    };

private:
    u8 _0[0xf - 0x0];
};
static_assert(sizeof(MenuInputManager) == 0xf);

class MultiControlInputManager : public MenuInputManager {
public:
    enum class WrappingMode {
        Both,
        Y,
        Neither,
    };

    class IInputHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(u32 localPlayerId) = 0;
    };

    template <typename T>
    class InputHandler : public IInputHandler {
    public:
        InputHandler(T *object, void (T::*function)(u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(u32 localPlayerId) override {
            (m_object->*m_function)(localPlayerId);
        }

    private:
        T *m_object;
        void (T::*m_function)(u32);
    };

    MultiControlInputManager();
    ~MultiControlInputManager();
    void dt(s32 type);
    void init(u32 playerFlags, bool isMultiPlayer);
    void setWrappingMode(WrappingMode wrappingMode);
    // TODO remaining args
    void setHandler(InputId inputId, IInputHandler *handler, bool r6, bool r7);

private:
    u8 _00f[0x224 - 0x00f];
};
static_assert(sizeof(MultiControlInputManager) == 0x224);

} // namespace UI
