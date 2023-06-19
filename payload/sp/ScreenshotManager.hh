#pragma once

#include <egg/core/eggColorFader.hh>
#include <egg/core/eggXfb.hh>

extern "C" {
#include <revolution/os.h>
#include <vendor/ff/ffconf.h>
}

namespace SP {

class ScreenshotManager {
public:
    void calc();
    void draw();

    static ScreenshotManager *CreateInstance(u32 framebufferSize);
    static ScreenshotManager *Instance();

private:
    struct FramebufferInfo {
        void *framebuffer = nullptr;
        u32 framebufferSize = 0;
    };

    ScreenshotManager(u32 framebufferSize);
    ScreenshotManager(const ScreenshotManager &) = delete;
    ScreenshotManager(ScreenshotManager &&) = delete;

    void save();
    void capture(EGG::Xfb *xfb);

    static void *SaveTask(void *arg);

    bool m_saving = false;
    std::array<wchar_t, FF_MAX_LFN + 1> m_screenshotFilepath;
    FramebufferInfo m_framebufferInfo;
    EGG::ColorFader m_colorFader{0.0f, 0.0f, 640.0f, 480.0f, nw4r::ut::Color::White,
            EGG::ColorFader::Status::Hidden};

    std::array<u8, 0x2000 /* 8 KiB */> m_stack{};
    OSThread m_thread;

    static ScreenshotManager *s_instance;
};

} // namespace SP
