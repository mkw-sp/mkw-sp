#include "Stack.h"

#include <revolution/os/OSCache.h>

typedef bool (*Find)(u32 *startAddress);
typedef void (*LinkRegisterFunction)(void);

typedef struct LinkRegisterPatch {
    Find findPrologue;
    Find findLRSave;
    Find findLRRestore;
    Find findEpilogue;

    u32 prologueInstCount;
    u32 epilogueInstCount;

    LinkRegisterFunction newLRSaveFunc;
    LinkRegisterFunction newLRRestoreFunc;
} LinkRegisterPatch;

void Stack_XORAlignedLinkRegisterSave(void);
void Stack_XORAlignedLinkRegisterRestore(void);
void Stack_XORLinkRegisterSave(void);
void Stack_XORLinkRegisterRestore(void);

static const u32 blr = 0x4E800020;
static const u32 mflr = 0x7C0802A6;
static const u32 mtlr = 0x7C0803A6;

u32 __stack_chk_guard;

void Stack_InitCanary(void) {
    __stack_chk_guard = (__builtin_ppc_mftb() & 0x00FFFFFF) | (0x80 << 24);
}

__attribute__((noreturn)) void __stack_chk_fail(void) {
    panic("Stack smashing detected!");
    __builtin_unreachable();
}

static bool IsAlignedPrologue(u32 *startAddress) {
    const u32 stwux = 0x7C21596E;

    return startAddress[0] == stwux && startAddress[1] == mflr;
}

static bool IsAlignedLinkRegisterSave(u32 *startAddress) {
    const u32 stw = 0x900C0004;

    return startAddress[0] == stw;
}

static bool IsAlignedLinkRegisterRestore(u32 *startAddress) {
    const u32 lwz = 0x800A0004;

    return startAddress[0] == lwz;
}

static bool IsAlignedEpilogue(u32 *startAddress) {
    const u32 mr = 0x7D415378;

    return startAddress[0] == mtlr && startAddress[1] == mr && startAddress[2] == blr;
}

static bool IsPrologue(u32 *startAddress) {
    const u16 stwu = 0x9421;

    return (startAddress[0] >> 16) == stwu && startAddress[1] == mflr;
}

static bool IsLinkRegisterSave(u32 *startAddress) {
    const u16 stw = 0x9001;

    return (startAddress[0] >> 16) == stw;
}

static bool IsLinkRegisterRestore(u32 *startAddress) {
    const u16 lwz = 0x8001;

    return (startAddress[0] >> 16) == lwz;
}

static bool IsEpilogue(u32 *startAddress) {
    const u16 addi = 0x3821;

    return startAddress[0] == mtlr && (startAddress[1] >> 16) == addi && startAddress[2] == blr;
}

// clang-format off
static const LinkRegisterPatch lrPatches[2] = {
        {
                .findPrologue = IsPrologue,
                .findLRSave = IsLinkRegisterSave,
                .findLRRestore = IsLinkRegisterRestore,
                .findEpilogue = IsEpilogue,

                .prologueInstCount = 2,
                .epilogueInstCount = 3,

                .newLRSaveFunc = Stack_XORLinkRegisterSave,
                .newLRRestoreFunc = Stack_XORLinkRegisterRestore,
        },
        {
                .findPrologue = IsAlignedPrologue,
                .findLRSave = IsAlignedLinkRegisterSave,
                .findLRRestore = IsAlignedLinkRegisterRestore,
                .findEpilogue = IsAlignedEpilogue,

                .prologueInstCount = 2,
                .epilogueInstCount = 3,

                .newLRSaveFunc = Stack_XORAlignedLinkRegisterSave,
                .newLRRestoreFunc = Stack_XORAlignedLinkRegisterRestore
        },
};
// clang-format on

static u32 *FindFirst(u32 *startAddress, u32 *endAddress, Find find, u32 instCount) {
    while (startAddress + instCount <= endAddress) {
        if (find(startAddress)) {
            return startAddress;
        }
        startAddress++;
    }

    return NULL;
}

static u32 *FindLast(u32 *startAddress, u32 *endAddress, Find find, u32 instCount) {
    while (endAddress - instCount >= startAddress) {
        if (find(endAddress - instCount)) {
            return endAddress - instCount;
        }
        endAddress--;
    }

    return NULL;
}

static u32 CreateBranchLinkInstruction(u32 *sourceAddress, u32 *destinationAddress) {
    return (18 << 26) | (((u32)destinationAddress - (u32)sourceAddress) & 0x3FFFFFC) | (1 << 0);
}

void Stack_DoLinkRegisterPatches(u32 *start, u32 *end) {
    assert(((u32)start & 3) == 0);
    assert(((u32)end & 3) == 0);

    for (size_t n = 0; n < ARRAY_SIZE(lrPatches); n++) {
        const LinkRegisterPatch *lrPatch = &lrPatches[n];
        u32 *startAddress = start;
        u32 *endAddress = end;

        while (startAddress < endAddress) {
            u32 *prologue = FindFirst(startAddress, endAddress, lrPatch->findPrologue,
                    lrPatch->prologueInstCount);
            if (!prologue) {
                break;
            }

            u32 *epilogue = FindFirst(prologue + lrPatch->prologueInstCount, endAddress,
                    lrPatch->findEpilogue, lrPatch->epilogueInstCount);
            if (!epilogue) {
                break;
            }

            u32 *lrSave = FindFirst(prologue + lrPatch->prologueInstCount, epilogue,
                    lrPatch->findLRSave, 1);
            if (!lrSave) {
                goto LcheckNextFunction;
            }

            u32 *lrRestore = FindLast(lrSave + 1, epilogue, lrPatch->findLRRestore, 1);
            if (!lrRestore) {
                goto LcheckNextFunction;
            }

            // Skip over functions that do not return
            u32 *nextPrologue = FindFirst(prologue + lrPatch->prologueInstCount, endAddress,
                    lrPatch->findPrologue, lrPatch->prologueInstCount);
            if (nextPrologue && nextPrologue < epilogue) {
                startAddress = nextPrologue;
                continue;
            }

            *lrSave = CreateBranchLinkInstruction(lrSave, (u32 *)lrPatch->newLRSaveFunc);
            *lrRestore = CreateBranchLinkInstruction(lrRestore, (u32 *)lrPatch->newLRRestoreFunc);

            DCFlushRange(lrSave, sizeof(*lrSave));
            DCFlushRange(lrRestore, sizeof(*lrRestore));
            ICInvalidateRange(lrSave, sizeof(*lrSave));
            ICInvalidateRange(lrRestore, sizeof(*lrRestore));

        LcheckNextFunction:
            startAddress = epilogue + lrPatch->epilogueInstCount;
        }
    }
}
