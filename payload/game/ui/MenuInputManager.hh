#pragma once

#include <Common.hh>

namespace UI {

class MenuInputManager {
public:
    enum class InputId {
        Front,
        Back,
        Option,
    };

    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(u32 localPlayerId) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(u32)) {
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

    MenuInputManager();
    ~MenuInputManager();
    void dt(s32 type);
    void init(u32 playerFlags, bool isMultiPlayer);

private:
    u8 _0[0xf - 0x0];
};
static_assert(sizeof(MenuInputManager) == 0xf);

class PageInputManager : public MenuInputManager {
public:
    PageInputManager();
    ~PageInputManager();
    void dt(s32 type);
    void init(u32 playerFlags, bool isMultiPlayer);
    void setHandler(InputId inputId, IHandler *handler, bool repeat);

private:
    u8 _00f[0x144 - 0x00f];
};
static_assert(sizeof(PageInputManager) == 0x144);

class MultiControlInputManager;

// TODO: Is this inherited from MenuInputManager?
class ControlInputManager {
public:
    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(u32 localPlayerId) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(u32)) {
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

    ControlInputManager();
    ~ControlInputManager();
    void setHandler(MenuInputManager::InputId inputId, IHandler *handler, bool repeat);

    struct ControlInputManagerPane {
        u8 _00[0x18 - 0x00];
        IHandler *selectHandler;
        u8 _1c[0x28 - 0x1c];
    };
    static_assert(sizeof(ControlInputManagerPane) == 0x28);

    u8 _00[0x0c - 0x00];
    ControlInputManagerPane m_pane;
    u8 _34[0x3c - 0x34];
    MultiControlInputManager *m_parent;
    u8 _40[0x75 - 0x40];

public:
    bool m_pointerOnly;

private:
    u8 _76[0x84 - 0x76];
};
static_assert(sizeof(ControlInputManager) == 0x84);

class MultiControlInputManager : public MenuInputManager {
public:
    enum class WrappingMode {
        Both,
        Y,
        Neither,
    };

    MultiControlInputManager();
    ~MultiControlInputManager();
    void dt(s32 type);
    void init(u32 playerFlags, bool isMultiPlayer);
    void setWrappingMode(WrappingMode wrappingMode);
    // TODO remaining args
    void setHandler(InputId inputId, IHandler *handler, bool r6 = false, bool r7 = false);
    void reinit(u32 playerFlags, u32 playerFlags2, bool isMultiPlayer);
    // TODO r6
    void select(u32 localPlayerId, ControlInputManager *child, s32 r6);

private:
    u8 _00f[0x224 - 0x00f];
};
static_assert(sizeof(MultiControlInputManager) == 0x224);

} // namespace UI
