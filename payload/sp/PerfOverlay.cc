#include "PerfOverlay.hh"

#include "sp/ScopeLock.hh"

#include <egg/core/eggSystem.hh>
#include <game/system/SaveManager.hh>
extern "C" {
#include <revolution.h>
}

#include <algorithm>
#include <cstring>
#include <iterator>

namespace SP {

void PerfOverlay::MeasureBeginFrame(OSTime frameDuration) {
    auto *saveManager = System::SaveManager::Instance();
    if (saveManager) {
        ScopeLock<NoInterrupts> lock;

        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::PerfOverlay>();
        bool enabled = setting == SP::ClientSettings::PerfOverlay::Enable;
        if (enabled && !s_instance) {
            s_instance = PerfOverlay();
        } else if (!enabled && s_instance) {
            s_instance.reset();
        }
    }

    if (s_instance) {
        s_instance->measureBeginFrame(frameDuration);
    }
}

void PerfOverlay::MeasureBeginRender() {
    if (s_instance) {
        s_instance->measureBeginRender();
    }
}

void PerfOverlay::Draw() {
    if (s_instance) {
        s_instance->draw();
    }
}

void PerfOverlay::MeasureEndRender() {
    if (s_instance) {
        s_instance->measureEndRender();
    }
}

void PerfOverlay::MeasureBeginCalc() {
    if (s_instance) {
        s_instance->measureBeginCalc();
    }
}

void PerfOverlay::MeasureEndCalc() {
    if (s_instance) {
        s_instance->measureEndCalc();
    }
}

PerfOverlay::PerfOverlay() {
    m_mainThread = OSGetCurrentThread();
    auto callback = OSSetSwitchThreadCallback(SwitchThreadCallback);
    if (callback && callback != SwitchThreadCallback) {
        s_switchThreadCallback = callback;
    }

    GXSetDrawSyncCallback(DrawSyncCallback);
}

PerfOverlay::~PerfOverlay() = default;

void PerfOverlay::measureBeginFrame(OSTime frameDuration) {
    m_frameDuration = frameDuration;
    m_frameStart = OSGetTime();

    {
        ScopeLock<NoInterrupts> lock;

        for (size_t i = 0; i < std::size(m_threads); i++) {
            if (m_threads[i] == m_mainThread) {
                m_threadColors[i] = {255, 80, 80, 255};
            } else if (m_threads[i]) {
                m_threadColors[i] = {255, 255, 255, 255};
            } else {
                m_threadColors[i] = {0, 0, 0, 0};
            }
        }
        m_lastThread = OSGetCurrentThread();
        m_lastThreadIndex = 0;
        std::fill(std::begin(m_threads), std::end(m_threads), nullptr);
    }

    m_gpuWidth = 600 * m_gpuDuration / m_frameDuration;

    for (size_t i = 0; i < std::size(m_memColors); i++) {
        auto &system = EGG::TSystem::Instance();
        u32 lo = reinterpret_cast<u32>(i == 0 ? system.mem1ArenaLo() : system.mem2ArenaLo());
        u32 hi = reinterpret_cast<u32>(i == 0 ? system.mem1ArenaHi() : system.mem2ArenaHi());
        u32 size = hi - lo;

        MEMHeapHandle lastHandle = nullptr;
        u8 colorId = 0;
        for (size_t j = 0; j < std::size(m_memColors[i]); j++) {
            ScopeLock<NoInterrupts> lock;

            u32 address = lo + j * (size / std::size(m_memColors[i]));
            const void *ptr = reinterpret_cast<void *>(address);
            MEMHeapHandle handle = MEMFindContainHeap(ptr);
            if (handle) {
                if (!MEMExIsAllocatedFromHeap(handle, ptr)) {
                    handle = nullptr;
                }
            }
            if (handle) {
                if (handle != lastHandle) {
                    colorId ^= 1;
                }
                if (colorId == 0) {
                    m_memColors[i][j] = {80, 255, 255, 255};
                } else {
                    m_memColors[i][j] = {255, 255, 80, 255};
                }
                lastHandle = handle;
            } else {
                m_memColors[i][j] = {0, 0, 0, 0};
            }
        }
    }
}

void PerfOverlay::measureBeginRender() {
    m_cpuDrawStart = OSGetTime() - m_frameStart;
    m_cpuDrawX = 4 + 600 * m_cpuDrawStart / m_frameDuration;

    {
        ScopeLock<NoInterrupts> lock;

        GXSetDrawSync(0);
    }
}

void PerfOverlay::draw() {
    GXSetViewport(0.0f, 0.0f, 608.0f, 456.0f, 0.0f, 1.0f);
    GXSetScissor(0, 0, 608, 456);
    float projMtx[4][4];
    C_MTXOrtho(projMtx, 0.0f, 456.0f, 0.0f, 608.0f, 0.0f, -1.0f);
    GXSetProjection(projMtx, GX_ORTHOGRAPHIC);
    float posMtx[3][4];
    PSMTXIdentity(posMtx);
    GXLoadPosMtxImm(posMtx, 0);
    GXSetCurrentMtx(0);

    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumChans(1);
    GXSetNumIndStages(0);
    GXSetNumTexGens(0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEX_DISABLE, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetNumTevStages(1);
    GXSetCullMode(GX_CULL_BACK);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GXSetColorUpdate(GX_TRUE);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);

    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);

    DrawRectangle(4, 432, 600, 6, {0, 0, 0, 102});
    DrawRectangle(m_cpuDrawX, 433, m_cpuDrawWidth, 2, {80, 255, 80, 255});
    DrawRectangle(m_cpuCalcX, 433, m_cpuCalcWidth, 2, {255, 80, 255, 255});
    DrawRectangles(435, m_threadColors);

    DrawRectangle(4, 440, 600, 4, {0, 0, 0, 102});
    DrawRectangle(m_gpuX, 441, m_gpuWidth, 2, {80, 80, 255, 255});

    DrawRectangle(4, 446, 600, 6, {0, 0, 0, 102});
    for (size_t i = 0; i < std::size(m_memColors); i++) {
        DrawRectangles(447 + i * 2, m_memColors[i]);
    }
}

void PerfOverlay::measureEndRender() {
    OSTime cpuDrawDuration = OSGetTime() - m_frameStart - m_cpuDrawStart;
    m_cpuDrawWidth = 600 * cpuDrawDuration / m_frameDuration;

    {
        ScopeLock<NoInterrupts> lock;

        GXSetDrawSync(1);
    }
}

void PerfOverlay::measureBeginCalc() {
    m_cpuCalcStart = OSGetTime() - m_frameStart;
    m_cpuCalcX = 4 + 600 * m_cpuCalcStart / m_frameDuration;
}

void PerfOverlay::measureEndCalc() {
    OSTime cpuCalcDuration = OSGetTime() - m_frameStart - m_cpuCalcStart;
    m_cpuCalcWidth = 600 * cpuCalcDuration / m_frameDuration;
}

void PerfOverlay::switchThreadCallback(OSThread *from, OSThread *to) {
    size_t index = 600 * (OSGetTime() - m_frameStart) / m_frameDuration;
    if (index > std::size(m_threads)) {
        return;
    }
    if (from == m_lastThread) {
        for (size_t i = m_lastThreadIndex; i < index; i++) {
            m_threads[i] = from;
        }
    }
    m_lastThread = to;
    m_lastThreadIndex = index;
}

void PerfOverlay::drawSyncCallback(u16 token) {
    if (token == 0) {
        m_gpuStart = OSGetTime() - m_frameStart;
        m_gpuX = 4 + 600 * m_gpuStart / m_frameDuration;
        // If there is very little to render, we might miss the end interrupt.
        m_gpuDuration = 0;
    } else {
        m_gpuDuration = OSGetTime() - m_frameStart - m_gpuStart;
    }
}

void PerfOverlay::DrawRectangle(s16 x, s16 y, s16 width, s16 height, GXColor color) {
    GXSetChanMatColor(GX_COLOR0A0, color);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition2s16(x, y);
        GXPosition2s16(x + width, y);
        GXPosition2s16(x + width, y + height);
        GXPosition2s16(x, y + height);
    }
    GXEnd();
}

void PerfOverlay::DrawRectangles(s16 y, GXColor (&colors)[600]) {
    size_t start = 0;
    for (size_t i = 0; i <= std::size(colors); i++) {
        if (i == std::size(colors) || memcmp(&colors[i], &colors[start], sizeof(GXColor))) {
            s16 width = i - start;
            if (width > 0 && colors[start].a > 0) {
                DrawRectangle(4 + start, y, width, 2, colors[start]);
            }
            start = i;
        }
    }
}

void PerfOverlay::SwitchThreadCallback(OSThread *from, OSThread *to) {
    if (s_switchThreadCallback) {
        s_switchThreadCallback(from, to);
    }

    if (s_instance) {
        s_instance->switchThreadCallback(from, to);
    }
}

void PerfOverlay::DrawSyncCallback(u16 token) {
    if (s_instance) {
        s_instance->drawSyncCallback(token);
    }
}

std::optional<PerfOverlay> PerfOverlay::s_instance{};
OSSwitchThreadCallback PerfOverlay::s_switchThreadCallback = nullptr;

} // namespace SP
