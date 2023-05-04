#include "dbException.hh"

extern "C" {
#include <revolution/os.h>
#include <sp/Host.h>
}
#include <sp/MapFile.hh>
extern "C" {
#include <sp/security/StackCanary.h>
}

namespace nw4r::db {

struct ExceptionInfo {
    OSThread osThread;
    OSMessageQueue osMessageQueue;
    void *framebuffer;
    u32 sp;
    ConsoleHandle consoleHandle;
    const GXRenderModeObj *gxRenderModeObj;
    ExceptionUserCallback exceptionUserCallback;
    void *exceptionUserCallbackArg;
    u32 msr;
    u32 fpscr;
    u16 displayInfo;
    u8 _35A[0x360 - 0x35A];
};
static_assert(sizeof(ExceptionInfo) == 0x360);

extern ExceptionInfo sExceptionInfo;

void Exception_Printf_(const char *format, ...);
void ShowMainInfo_(OSError osError, const OSContext *osContext, u32 dsisr, u32 dar);
void ShowFloat_(const OSContext *osContext);

static const u32 MEMORY_OFFSET_MASK = 0x0FFFFFFF;

static bool IsAddressInCachedMEM1(u32 address) {
    return address >= OS_CACHED_MEMORY_BASE &&
            address < OS_CACHED_MEMORY_BASE + OSGetPhysicalMem1Size();
}

static bool IsAddressInMEM1(u32 address) {
    if (!OSIsMEM1Region(address)) {
        return false;
    }

    return (address & MEMORY_OFFSET_MASK) < OSGetPhysicalMem1Size();
}

static bool IsAddressInMEM2(u32 address) {
    if (!OSIsMEM2Region(address)) {
        return false;
    }

    return (address & MEMORY_OFFSET_MASK) < OSGetPhysicalMem2Size();
}

static bool IsValidStackAddr_(u32 address) {
    if (address == 0x00000000 || address == 0xFFFFFFFF) {
        return false;
    }

    return IsAddressInMEM1(address) || IsAddressInMEM2(address);
}

static bool ShowMapInfoSubroutine_(u32 address) {
    if (!IsAddressInMEM1(address)) {
        return false;
    }

    const u32 symbolNameBufferSize = 75 - 15 + 1;
    char symbolNameBuffer[symbolNameBufferSize];
    if (!SP::MapFile::FindSymbol(address, symbolNameBuffer, symbolNameBufferSize)) {
        return false;
    }

    Exception_Printf_("%s\n", symbolNameBuffer);
    return true;
}

static void ShowGPR_(const OSContext *osContext) {
    Exception_Printf_("-------------------------------- GPR\n");

    for (int r = 0; r < 10; r++) {
        Exception_Printf_("R%02d:%08XH  R%02d:%08XH  R%02d:%08XH\n", r, osContext->gprs[r], r + 11,
                osContext->gprs[r + 11], r + 22, osContext->gprs[r + 22]);
    }
    Exception_Printf_("R10:%08XH  R21:%08XH\n", osContext->gprs[10], osContext->gprs[21]);
}

static void ShowGPRMap_(const OSContext *osContext) {
    Exception_Printf_("-------------------------------- GPRMAP\n");

    for (int r = 0; r < 32; r++) {
        u32 address = osContext->gprs[r];
        if (!IsAddressInCachedMEM1(address)) {
            continue;
        }

        Exception_Printf_("R%02d: %08XH ", r, address);
        if (!ShowMapInfoSubroutine_(address)) {
            Exception_Printf_("<Symbol not found>\n");
        }
    }
}

static void ShowSRR0Map_(const OSContext *osContext) {
    Exception_Printf_("-------------------------------- SRR0MAP\n");

    u32 srr0 = osContext->srr0;
    Exception_Printf_("SRR0: %08XH ", srr0);
    if (!ShowMapInfoSubroutine_(srr0)) {
        Exception_Printf_("<Symbol not found>\n");
    }
}

static void ShowStackTrace_(u32 sp) {
    Exception_Printf_("-------------------------------- TRACE\n");
    Exception_Printf_("Address:   Back chain   LR save\n");

    u32 *frame = reinterpret_cast<u32 *>(sp);

    for (int i = 0; i < 16; i++) {
        if (!IsValidStackAddr_((u32)frame)) {
            break;
        }

        u32 lr = frame[1];
        if (StackCanary_IsLinkRegisterEncrypted(lr)) {
            lr = StackCanary_XORLinkRegister(lr);
        }

        Exception_Printf_("%08X:  %08X     %08X ", frame, frame[0], lr);
        if (!ShowMapInfoSubroutine_(lr)) {
            Exception_Printf_("\n");
        }

        frame = reinterpret_cast<u32 *>(*frame);
    }
}

REPLACE void PrintContext_(OSError osError, const OSContext *osContext, u32 dsisr, u32 dar) {
    Exception_Printf_("******** %s ********\n",
            osError < OS_ERROR_MAX ? "EXCEPTION OCCURRED" : "USER HALT");
    Host_PrintMkwSpInfo(Exception_Printf_);
    Exception_Printf_("Framebuffer: %08XH\n", sExceptionInfo.framebuffer);
    Exception_Printf_("--------------------------------\n");

    if (sExceptionInfo.displayInfo & EXCEPTION_INFO_MAIN) {
        Exception_Printf_("---EXCEPTION_INFO_MAIN---\n");
        ShowMainInfo_(osError, osContext, dsisr, dar);
    }
    if (sExceptionInfo.displayInfo & EXCEPTION_INFO_GPR) {
        Exception_Printf_("---EXCEPTION_INFO_GPR---\n");
        ShowGPR_(osContext);
    }
    if (sExceptionInfo.displayInfo & EXCEPTION_INFO_SRR0MAP) {
        Exception_Printf_("---EXCEPTION_INFO_SRR0MAP---\n");
        ShowSRR0Map_(osContext);
    }
    if (sExceptionInfo.displayInfo & EXCEPTION_INFO_GPRMAP) {
        Exception_Printf_("---EXCEPTION_INFO_GPRMAP---\n");
        ShowGPRMap_(osContext);
    }
    if (sExceptionInfo.displayInfo & EXCEPTION_INFO_FPR) {
        Exception_Printf_("---EXCEPTION_INFO_FPR---\n");
        ShowFloat_(osContext);
        Exception_Printf_("MSR:%08XH  FPSCR:%08XH\n", sExceptionInfo.msr, sExceptionInfo.fpscr);
    }
    if (sExceptionInfo.displayInfo & EXCEPTION_INFO_TRACE) {
        Exception_Printf_("---EXCEPTION_INFO_TRACE---\n");
        ShowStackTrace_(sExceptionInfo.sp);
    }

    Exception_Printf_("--------------------------------\n");
}

} // namespace nw4r::db
