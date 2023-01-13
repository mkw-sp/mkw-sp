#pragma once

#include "game/ui/Page.hh"

extern "C" {
#include <revolution.h>
}

namespace UI {

class ChannelPage : public Page {
public:
    ChannelPage();
    ChannelPage(const ChannelPage &) = delete;
    ChannelPage(ChannelPage &&) = delete;
    ~ChannelPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onDeinit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void beforeOutAnim() override;
    void afterCalc() override;
    void onRefocus() override;

private:
    enum class State {
        Prev,
        Explanation,
        None,
        Older,
        Same,
        Newer,
        Install,
        Update,
        InstallOk,
        InstallFail,
        UpdateOk,
        UpdateFail,
        Unsupported,
        Next,
    };

    State resolve();
    void transition(State state);

    static void *Install(void *arg);

    MenuInputManager m_inputManager;
    PageId m_replacement;
    State m_state;
    u8 m_stack[0x6000 /* 24 KiB */];
    OSThread m_thread;
};

} // namespace UI
