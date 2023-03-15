#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/UIControl.hh"

#include <optional>

namespace UI {

class ScrollBar : public LayoutUIControl {
public:
    class IHandler {
    public:
        virtual void handle(ScrollBar *scrollBar, u32 localPlayerId, u32 chosen) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(ScrollBar *, u32, u32)) {
            m_object = object;
            m_function = function;
        }

        void handle(ScrollBar *scrollBar, u32 localPlayerId, u32 chosen) override {
            (m_object->*m_function)(scrollBar, localPlayerId, chosen);
        }

    private:
        T *m_object;
        void (T::*m_function)(ScrollBar *, u32, u32);
    };

    ScrollBar();
    ~ScrollBar() override;
    void calcSelf() override;

    void load(u32 count, u32 chosen, const char *dir, const char *file, const char *variant,
            u32 playerFlags, bool isMultiPlayer, bool pointerOnly);
    void setChangeHandler(IHandler *handler);
    void setPlayerFlags(u32 flags);
    void reconfigure(u32 count, u32 chosen, u32 playerFlags);

private:
    struct Sequence {
        u32 localPlayerId;
        u32 flags;
        u32 frames;
    };

    void onSelect(u32 localPlayerId, u32 r5);
    void onDeselect(u32 localPlayerId, u32 r5);
    void onFront(u32 localPlayerId, u32 r5);
    void onRight(u32 localPlayerId, u32 r5);
    void onLeft(u32 localPlayerId, u32 r5);

    template <typename T>
    using H = typename T::Handler<ScrollBar>;

    ControlInputManager m_inputManager;
    H<ControlInputManager> m_onSelect{this, &ScrollBar::onSelect};
    H<ControlInputManager> m_onDeselect{this, &ScrollBar::onDeselect};
    H<ControlInputManager> m_onFront{this, &ScrollBar::onFront};
    H<ControlInputManager> m_onRight{this, &ScrollBar::onRight};
    H<ControlInputManager> m_onLeft{this, &ScrollBar::onLeft};
    IHandler *m_changeHandler = nullptr;
    nw4r::lyt::Pane *m_fuchiPatternPane;
    nw4r::lyt::Pane *m_colorBasePane;
    nw4r::lyt::Pane *m_thumbFuchiPane;
    nw4r::lyt::Pane *m_thumbPane;
    u32 m_count;

public:
    u32 m_chosen;

private:
    std::optional<Sequence> m_sequence{};
};

} // namespace UI
