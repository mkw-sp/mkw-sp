#include "eggException.hh"

#include "egg/core/eggThread.hh"

#include <common/Clock.hh>
#include <game/host_system/SystemManager.hh>
extern "C" {
#include <revolution/ax.h>
#include <revolution/kpad.h>
#include <revolution/os.h>
#include <revolution/pad.h>
#include <revolution/vi.h>
}

#include <cstring>

#define EXCEPTION_CONSOLE_WIDTH 75
#define EXCEPTION_CONSOLE_HEIGHT 98

#define EXCEPTION_CONSOLE_ATTRIBUTE_NO_LINE_WRAP (1 << 0)
#define EXCEPTION_CONSOLE_ATTRIBUTE_TAB_SIZE_4 (1 << 2)
#define EXCEPTION_CONSOLE_ATTRIBUTES \
    EXCEPTION_CONSOLE_ATTRIBUTE_NO_LINE_WRAP | EXCEPTION_CONSOLE_ATTRIBUTE_TAB_SIZE_4

namespace EGG {

static constexpr bool CheckGCStickThreshold(s8 stick) {
    return (stick >= 27 || stick <= -26);
}

static constexpr bool CheckCLStickThreshold(s16 stick) {
    return (stick >= 161 || stick <= -160);
}

bool ExceptionCallBack_(nw4r::db::ConsoleHandle console, void * /* arg */) {
    OSReport("CALLBACK...\n");

    AXSetMasterVolume(0);

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

    u32 controllerType;
    s32 err = WPADProbe(0, &controllerType);
    bool classic = (err == 0 && controllerType == 2);

    while (true) {
        KPADStatus wStatus{};
        PADStatus gcStatus[4]{};
        WPADCLStatus clStatus{};

        KPADRead(0, &wStatus, 1);
        PADRead(gcStatus);
        PADClampCircle(gcStatus);
        if (classic) {
            KPADGetUnifiedWpadStatus(0, &clStatus, 1);
        }

        if (OSGetCurrentThread()) {
            if (wStatus.buttons & KPAD_BUTTON_HOME || gcStatus[0].buttons & PAD_BUTTON_START ||
                    (classic && clStatus.buttons & WPAD_CL_BUTTON_HOME)) {
                System::SystemManager::Restart();
            }
        }

        bool left = (wStatus.buttons & KPAD_BUTTON_LEFT || gcStatus[0].buttons & PAD_BUTTON_LEFT);
        bool right = (!!(wStatus.buttons & KPAD_BUTTON_RIGHT) ||
                !!(gcStatus[0].buttons & PAD_BUTTON_RIGHT));
        bool down = (!!(wStatus.buttons & KPAD_BUTTON_DOWN) ||
                !!(gcStatus[0].buttons & PAD_BUTTON_DOWN));
        bool up = (!!(wStatus.buttons & KPAD_BUTTON_UP) || !!(gcStatus[0].buttons & PAD_BUTTON_UP));

        if (classic) {
            up |= !!(clStatus.buttons & WPAD_CL_BUTTON_UP);
            left |= !!(clStatus.buttons & WPAD_CL_BUTTON_LEFT);
            down |= !!(clStatus.buttons & WPAD_CL_BUTTON_DOWN);
            right |= !!(clStatus.buttons & WPAD_CL_BUTTON_RIGHT);
        }

        if (CheckGCStickThreshold(gcStatus[0].stickX)) {
            if (gcStatus[0].stickX < 0) {
                left = true;
            } else {
                right = true;
            }
        }

        if (CheckGCStickThreshold(gcStatus[0].stickY)) {
            if (gcStatus[0].stickY < 0) {
                down = true;
            } else {
                up = true;
            }
        }

        if (classic) {
            if (CheckCLStickThreshold(clStatus.lStickX)) {
                if (clStatus.lStickX < 0) {
                    left = true;
                } else {
                    right = true;
                }
            }

            if (CheckCLStickThreshold(clStatus.lStickY)) {
                if (clStatus.lStickY < 0) {
                    down = true;
                } else {
                    up = true;
                }
            }
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

        if (right) {
            xPos = std::max(xPos - 5, -150);
        } else if (left) {
            xPos = std::min(xPos + 5, 10);
        }

        if (down) {
            currentTopLine = std::min(currentTopLine + 1, totalLines);
        } else if (up) {
            currentTopLine = std::max(currentTopLine - 1, lineCount);
        }

        if (currentTopLine != prevTopLine || xPos != prevXPos) {
            console->m_xPos = xPos;
            console->m_currentTopLine = currentTopLine;
            nw4r::db::Console_DrawDirect(console);
        }
    }
}

Exception *Exception::create(u16 /* width */, u16 /* height */, u16 /* attributes */, Heap *heap,
        u32 numMapFiles) {
    return REPLACED(create)(EXCEPTION_CONSOLE_WIDTH, EXCEPTION_CONSOLE_HEIGHT,
            EXCEPTION_CONSOLE_ATTRIBUTES, heap, numMapFiles);
}

void Exception::setUserCallBack(void * /* arg */) {
    REPLACED(setUserCallBack)(nullptr);
}

} // namespace EGG
