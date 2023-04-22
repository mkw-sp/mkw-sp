#pragma once

#include <revolution.hh>

#include <optional>

namespace SP {

class PerfOverlay {
public:
    ~PerfOverlay();
    static void MeasureBeginFrame(OSTime frameDuration);
    static void MeasureBeginRender();
    static void Draw();
    static void MeasureEndRender();
    static void MeasureBeginCalc();
    static void MeasureEndCalc();

private:
    PerfOverlay();
    void measureBeginFrame(OSTime frameDuration);
    void measureBeginRender();
    void draw();
    void measureEndRender();
    void measureBeginCalc();
    void measureEndCalc();
    void switchThreadCallback(OSThread *from, OSThread *to);
    void drawSyncCallback(u16 token);

    static void DrawRectangle(s16 x, s16 y, s16 width, s16 height, GXColor color);
    static void DrawRectangles(s16 y, GXColor (&colors)[600]);
    static void SwitchThreadCallback(OSThread *from, OSThread *to);
    static void DrawSyncCallback(u16 token);

    OSTime m_frameDuration = 0;
    OSTime m_frameStart = 0;
    OSTime m_cpuDrawStart = 0;
    s16 m_cpuDrawX = 0;
    s16 m_cpuDrawWidth = 0;
    OSTime m_cpuCalcStart = 0;
    s16 m_cpuCalcX = 0;
    s16 m_cpuCalcWidth = 0;
    OSThread *m_mainThread = nullptr;
    OSThread *m_lastThread = nullptr;
    size_t m_lastThreadIndex = 0;
    OSThread *m_threads[600];
    GXColor m_threadColors[600];
    OSTime m_gpuStart = 0;
    OSTime m_gpuDuration = 0;
    s16 m_gpuX = 0;
    s16 m_gpuWidth = 0;
    GXColor m_memColors[2][600];

    static std::optional<PerfOverlay> s_instance;
    static OSSwitchThreadCallback s_switchThreadCallback;
};

} // namespace SP
