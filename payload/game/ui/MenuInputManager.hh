#pragma once

#include "game/ui/TypeInfo.hh"

#include <nw4r/lyt/lyt_pane.hh>

namespace UI {

class MenuInputManager {
public:
    enum class InputId {
        Front = 0,
        Back = 1,
        Option = 2,
        Up = 3,
        Down = 4,
        Left = 5,
        Right = 6,
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
    virtual ~MenuInputManager();
    virtual TypeInfo *getTypeInfo() const;
    virtual void dt(s32 type);
    virtual void init(u32 playerFlags, bool isMultiPlayer);
    virtual void vf_14();
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();
    virtual void vf_24();
    virtual void vf_28();
    virtual void vf_2c();
    virtual void vf_30();
    virtual void vf_34();

    template <typename I>
    const I *downcast() const {
        for (TypeInfo *typeInfo = getTypeInfo(); typeInfo != nullptr; typeInfo = typeInfo->base) {
            if (typeInfo == I::GetTypeInfo()) {
                return reinterpret_cast<const I *>(this);
            }
        }
        return nullptr;
    }

    template <typename I>
    I *downcast() {
        for (TypeInfo *typeInfo = getTypeInfo(); typeInfo != nullptr; typeInfo = typeInfo->base) {
            if (typeInfo == I::GetTypeInfo()) {
                return reinterpret_cast<I *>(this);
            }
        }
        return nullptr;
    }

    static TypeInfo *GetTypeInfo();

private:
    u8 _4[0xf - 0x4];
};
static_assert(sizeof(MenuInputManager) == 0x10);

class PageInputManager : public MenuInputManager {
public:
    PageInputManager();
    ~PageInputManager() override;
    TypeInfo *getTypeInfo() const override;
    void dt(s32 type) override;
    void init(u32 playerFlags, bool isMultiPlayer) override;
    void vf_18() override;
    void vf_1c() override;
    void vf_24() override;

    void setHandler(InputId inputId, IHandler *handler, bool repeat);

    static TypeInfo *GetTypeInfo();

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
        virtual void handle(u32 localPlayerId, u32 r5) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(u32, u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(u32 localPlayerId, u32 r5) override {
            (m_object->*m_function)(localPlayerId, r5);
        }

    private:
        T *m_object;
        void (T::*m_function)(u32, u32);
    };

    ControlInputManager();
    ~ControlInputManager();
    void init(u32 childCount, bool isMultiPlayer, bool pointerOnly);
    void setHandler(MenuInputManager::InputId inputId, IHandler *handler, bool repeat);
    void getFuchiPatternColors(GXColorS10 *color0, GXColorS10 *color1) const;
    void getColorBaseColor(GXColorS10 *color);

    struct ControlInputManagerPane {
        nw4r::lyt::Pane *pane;
        u8 _04[0x18 - 0x04];
        IHandler *selectHandler;
        IHandler *deselectHandler;
        u8 _1c[0x28 - 0x20];
    };
    static_assert(sizeof(ControlInputManagerPane) == 0x28);

private:
    u8 _00[0x0c - 0x00];

public:
    ControlInputManagerPane m_pane;

private:
    u8 _34[0x3c - 0x34];

public:
    MultiControlInputManager *m_parent;

private:
    u8 _40[0x70 - 0x40];

public:
    u32 m_playerFlags;

private:
    u8 _70[0x75 - 0x74];

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
    ~MultiControlInputManager() override;
    TypeInfo *getTypeInfo() const override;
    void dt(s32 type) override;
    void init(u32 playerFlags, bool isMultiPlayer) override;
    void vf_14() override;
    void vf_18() override;
    void vf_1c() override;
    void vf_20() override;
    void vf_24() override;
    void vf_28() override;
    void vf_2c() override;
    void vf_30() override;
    void vf_34() override;

    u32 sequenceFlags(u32 localPlayerId) const;
    u32 sequenceFrames(u32 localPlayerId) const;
    bool isPointer(u32 localPlayerId) const;
    Vec2<f32> pointerPos(u32 localPlayerId) const;
    void setWrappingMode(WrappingMode wrappingMode);
    void append(ControlInputManager *child);
    // TODO remaining args
    void setHandler(InputId inputId, IHandler *handler, bool r6 = false, bool r7 = false);
    void reinit(u32 playerFlags, u32 playerFlags2, bool isMultiPlayer);
    // TODO r6
    void select(u32 localPlayerId, ControlInputManager *child, s32 r6);
    void setPerControl(u32 localPlayerId, bool perControl);
    bool isPerControl(u32 localPlayerId) const;

    static TypeInfo *GetTypeInfo();

private:
    class Player {
    public:
        u32 sequenceFlags() const;
        u32 sequenceFrames() const;
        bool isPointer() const;
        Vec2<f32> pointerPos() const;
        bool isPerControl() const;

    private:
        u8 _00[0x3c - 0x00];
        u32 m_sequenceFlags;
        u32 m_sequenceFrames;
        bool m_isPointer;
        u8 _45[0x48 - 0x45];
        Vec2<f32> m_pointerPos;
        bool m_isPerControl;
        u8 _51[0x5c - 0x51];
    };
    static_assert(sizeof(Player) == 0x5c);

    u8 _00f[0x054 - 0x00f];
    Player m_players[5];
    u8 _220[0x224 - 0x220];
};
static_assert(sizeof(MultiControlInputManager) == 0x224);

} // namespace UI
