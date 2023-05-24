extern "C" {
#include <revolution/os.h>
}
#include <sp/MapFile.hh>
extern "C" {
#include <sp/security/StackCanary.h>
}

static const u32 MEMORY_OFFSET_MASK = 0x0FFFFFFF;

static bool IsStackFrameInMEM1(u32 address) {
    if (!OSIsMEM1Region(address)) {
        return false;
    }

    return (address & MEMORY_OFFSET_MASK) <= OSGetPhysicalMem1Size() - (sizeof(u32) * 2);
}

static bool IsStackFrameInMEM2(u32 address) {
    if (!OSIsMEM2Region(address)) {
        return false;
    }

    return (address & MEMORY_OFFSET_MASK) <= OSGetPhysicalMem2Size() - (sizeof(u32) * 2);
}

static bool IsValidStackAddress(u32 address) {
    if (address == 0x00000000 || address == 0xFFFFFFFF) {
        return false;
    }

    return IsStackFrameInMEM1(address) || IsStackFrameInMEM2(address);
}

static bool ShowMapInfoSubroutine(u32 address) {
    if (!IsStackFrameInMEM1(address)) {
        return false;
    }

    const u32 symbolNameBufferSize = 512;
    char symbolNameBuffer[symbolNameBufferSize];
    if (!SP::MapFile::PrintAddressSymbolInfo(address, symbolNameBuffer, symbolNameBufferSize)) {
        return false;
    }

    OSReport("%s\n", symbolNameBuffer);
    return true;
}

extern "C" REPLACE void OSDumpContext(const OSContext *context) {
    OSReport("-------------------------------------\n");
    OSReport("-------- Context: 0x%08X --------\n", reinterpret_cast<u32>(context));
    OSReport("-------------------------------------\n");
    OSReport("LR   = 0x%08X  CR   = 0x%08X\n", context->lr, context->cr);
    OSReport("SRR0 = 0x%08X  SRR1 = 0x%08X\n", context->srr0, context->srr1);

    OSReport("\n");
    OSReport("----- General Purpose Registers -----\n");
    for (int i = 0; i < 16; i++) {
        OSReport("R%02d  = 0x%08X  R%02d  = 0x%08X\n", i, context->gprs[i], i + 16,
                context->gprs[i + 16]);
    }

    if (context->state & OS_CONTEXT_STATE_FLOATING_POINT_REGISTERS_SAVED) {
        BOOL enabled = OSDisableInterrupts();
        {
            OSContext temporaryContext;
            OSContext *currentContext = OSGetCurrentContext();

            OSClearContext(&temporaryContext);
            OSSetCurrentContext(&temporaryContext);

            OSReport("\n");
            OSReport("----- Floating-Point Registers ------\n");
            for (int i = 0; i < 16; i++) {
                OSReport("F%02d  = %11d F%02d  = %11d\n", i, static_cast<s32>(context->fprs[i]),
                        i + 16, static_cast<s32>(context->fprs[i + 16]));
            }

            OSClearContext(&temporaryContext);
            OSSetCurrentContext(currentContext);
        }
        OSRestoreInterrupts(enabled);
    }

    u32 *frame = reinterpret_cast<u32 *>(context->gprs[1]);

    OSReport("\n");
    OSReport("------------ Stack Trace ------------\n");
    OSReport("Address:      Back Chain   LR Save\n");
    for (int i = 0; i < 16; i++) {
        if (!IsValidStackAddress(reinterpret_cast<u32>(frame))) {
            break;
        }

        u32 lr = frame[1];
        if (StackCanary_IsLinkRegisterEncrypted(lr)) {
            lr = StackCanary_XORLinkRegister(lr);
        }

        OSReport("0x%08X:   0x%08X   0x%08X ", reinterpret_cast<u32>(frame), frame[0], lr);
        if (!ShowMapInfoSubroutine(lr)) {
            OSReport("\n");
        }

        frame = reinterpret_cast<u32 *>(*frame);
    }
}
