#include "eggException.hh"

#include <common/Clock.hh>

#include "egg/core/eggThread.hh"

extern "C" {
#include <revolution/kpad.h>
#include <revolution/os.h>
#include <revolution/pad.h>
#include <revolution/vi.h>
}

namespace EGG {

static constexpr bool CheckGCStickThreshold(s8 stick) {
    return (stick >= 27 || stick <= -26);
}

bool ExceptionCallBack_(nw4r::db::ConsoleHandle console) {
    OSReport("CALLBACK...\n");
    if (!console) {
        OSReport("No Console\n");
        return false;
    }

    VISetBlack(0);
    VIFlush();
    OSReport("cancel all thread...\n");
    Thread::kandoTestCancelAllThread();
    OSReport("done\n");

    OSDisableInterrupts();
    OSDisableScheduler();
    OSEnableInterrupts();

    s32 lineCount = console->m_lineCount;
    s32 totalLines = nw4r::db::Console_GetTotalLines(console);

    console->m_visible = true;
    console->m_currentTopLine = lineCount;
    nw4r::db::Console_DrawDirect(console);

    KPADStatus wStatus;
    PADStatus gcStatus[4];
    u32 input;

    while (true) {
        KPADRead(0, &wStatus, 1);
        PADRead(gcStatus);
        PADClampCircle(gcStatus);

        input = wStatus.buttons & 15;
        input |= gcStatus[0].buttons & 15;

        if (CheckGCStickThreshold(gcStatus[0].stickX)) {
            input |= (1 << !(gcStatus[0].stickX & 0x80));
        }

        if (CheckGCStickThreshold(gcStatus[0].stickY)) {
            input |= (1 << (!(gcStatus[0].stickY & 0x80) + 2));
        }

        u32 tick0 = OSGetTick();
        u32 tick1;
        do {
            tick1 = OSGetTick();
        } while (OSTicksToMilliseconds(tick1 - tick0) < 100);

        s32 xPos = console->m_xPos;
        s32 currentTopLine = console->m_currentTopLine;

        s32 prevXPos = xPos;
        s32 prevTopLine = currentTopLine;

        if (input & KPAD_BUTTON_RIGHT) {
            xPos = std::max(xPos - 5, -150);
        } else if (input & KPAD_BUTTON_LEFT) {
            xPos = std::min(xPos + 5, 10);
        }

        if (input & KPAD_BUTTON_DOWN) {
            currentTopLine = std::min(currentTopLine + 1, totalLines);
        } else if (input & KPAD_BUTTON_UP) {
            currentTopLine = std::max(currentTopLine - 1, lineCount);
        }

        if (currentTopLine != prevTopLine || xPos != prevXPos) {
            console->m_xPos = xPos;
            console->m_currentTopLine = currentTopLine;
            nw4r::db::Console_DrawDirect(console);
        }
    }
}

} // namespace EGG
