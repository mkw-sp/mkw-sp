#pragma once

#include "game/ui/MessagePage.hh"

extern "C" {
#include <revolution.h>
}

namespace UI {

class UpdatePage : public Page {
public:
    UpdatePage();
    UpdatePage(const UpdatePage &) = delete;
    UpdatePage(UpdatePage &&) = delete;
    ~UpdatePage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void beforeOutAnim() override;
    void afterCalc() override;
    void onRefocus() override;

private:
    enum class State {
        Prev,
        Check,
        CheckOk,
        CheckFail,
        ConfirmUpdate,
        Update,
        UpdateOk,
        UpdateFail,
        ConfirmRestart,
        Restart,
        Unsupported,
        Next,
    };

    void onCheckOkFront(MessagePage *messagePage);

    State resolve();
    void transition(State state);

    static void *Check(void *arg);
    static void *Update(void *arg);

    template <typename T>
    using H = typename T::Handler<UpdatePage>;

    MenuInputManager m_inputManager;
    H<MessagePage> m_onCheckOkFront{this, &UpdatePage::onCheckOkFront};
    PageId m_replacement;
    State m_state;
    u8 m_stack[0x4000 /* 16 KiB */];
    OSThread m_thread;
};

} // namespace UI
